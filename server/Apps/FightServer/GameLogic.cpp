﻿#include "GameLogic.h"


//////////////////////////////////////////////////////////////////////////
/// 游戏配置相关

// 服务端逻辑帧最多领先最快客户端多少帧
static const uint32_t MAX_LEAD_FRAME_DIS_MAX_CLIENT = 2U;

// 服务端逻辑帧最多领先最慢客户端多少帧 (超过这个帧数后切换为等待状态)
static const uint32_t MAX_LEAD_FRAME_DIS_MIN_CLIENT = 100U;

// 服务器处于 WAIT_CONNECT 阶段时最多等待客户端多少秒(超过这个时间客户端还没有连上就会将客户端踢出游戏)
static const float MAX_WAIT_CONNECT_TIME = 5.0f;

// 单个玩家游戏时,客户端最大离线时间(超过这个时间客户端还没有断线重连就会将客户端踢出游戏)
static const float MAX_OFF_LINE_TIME_SINGLE_PLAYER = 20.0f;

// 多个玩家游戏时,客户端最大离线时间(应该设置较小,避免其他玩家太长时间处于等待期)
static const float MAX_OFF_LINE_TIME_MULTIPLE_PLAYER = 5.0f;



// ping值采集间隔时间
static const float GET_PING_INTERVAL = 0.5f;

// ping值广播推送间隔时间
static const float PUSH_PING_INTERVAL = 1.0f;


// 操作记录最多保留帧数
static const int MAX_RECORDS_COUNT = 300;

//////////////////////////////////////////////////////////////////////////
static const G_BIT_TYPE G_KEY_EXIT_GAME = 1U << 13; // 玩家退出游戏
//////////////////////////////////////////////////////////////////////////


static int32_t sg_uuid_seed = 0;

GameLogic::GameLogic()
	: m_isFinish(false)
	, m_pNetService(NULL)
	, m_playerCount(0)
	, m_pApplication(NULL)
{
	for (int i = 0; i < G_FIGHT_MAX_PLAYER_COUNT; ++i)
	{
		m_pCacheFrameInputs[i] = NULL;
	}

	m_uuid = sg_uuid_seed++;
	m_state = RUN_STATE::WAIT_CONNECT;
	m_waitTime = 0.0f;
	m_lastRunTime = 0.0f;
	m_accumilatedTime = 0.0f;
	m_pingTime = 0.0f;
	m_pingPushTime = 0.0f;
}

GameLogic::~GameLogic()
{
	if (m_pNetService)
	{
		m_pNetService->noticeCenter()->delListener(this);
	}
}

err::Code GameLogic::init(const GGameWorldInitArgs &args, const ::google::protobuf::RepeatedPtrField< ::svr_msg::FightRoleSpawnInfo >& roles)
{
	this->setInitArgs(args);

	m_world = std::make_unique<GGameWorld>();

	if (m_world == NULL)
	{
		return err::Code::NO_MEMORY;
	}

	if (!m_world->init(args))
	{
		LOG(ERROR) << "init map failed, mapID: " << args.mapId;
		return err::Code::FIGHT_INIT_FAIL;
	}

	if (roles.size() > G_FIGHT_MAX_PLAYER_COUNT)
		return err::Code::FIGHT_PLAYER_TOO_MUCH;

	for (auto i = 0; i < roles.size(); ++i)
	{
		m_players[i] = std::make_unique<GamePlayer>();
		if (m_players[i] == NULL)
			return err::Code::NO_MEMORY;

		auto code = m_players[i]->init(roles.Get(i));
		if (code != err::Code::SUCCESS)
			return code;

		m_playerIDSet.insert(m_players[i]->getPlayerID());
	}
	if (m_playerIDSet.size() != roles.size())
	{
		return err::Code::FIGHT_PLAYERID_REPEAT;
	}

	m_pApplication = GApplication::getInstance();
	m_playerCount = roles.size();
	m_pNetService = m_pApplication->getServiceMgr()->getService<GNetService>();

	//! 客户端通信
	ON_PB_MSG_CLASS_CALL(m_pNetService->noticeCenter(), msg::RunNextFrameReq, onMsg_RunNextFrameReq);
	ON_PB_MSG_CLASS_CALL(m_pNetService->noticeCenter(), msg::PlayerLoadingReq, onMsg_PlayerLoadingReq);
	ON_PB_MSG_CLASS_CALL(m_pNetService->noticeCenter(), msg::Pong, onMsg_Pong);

	return err::Code::SUCCESS;
}

void GameLogic::update(float dt)
{
	// 玩家全部退出游戏,自动结束游戏
	if (m_playerCount <= 0)
	{
		this->setIsFinish(true);
		return;
	}

	pingUpdate(dt);

	switch (m_state)
	{
	case GameLogic::RUN_STATE::WAIT_CONNECT:
	{
		this->update_WaitConnect(dt);
	}break;
	case GameLogic::RUN_STATE::READY:
	{
		this->update_Ready(dt);
	}break;
	case GameLogic::RUN_STATE::RUN:
	{
		// 客户端使用离线模式(此模式只有单人才能开启)
		if (m_initArgs.isOfflineMode)
		{
			auto player = m_players[0].get();;
			if (player)
			{
				// 离线状态
				if (player->isOffline())
				{
					// 离线太久,踢出游戏
					if (m_pApplication->getRunTime() - player->getOfflineTime() > MAX_OFF_LINE_TIME_SINGLE_PLAYER)
					{
						LOG(INFO) << "Offline too long(1), forced exit, playerid: " << player->getPlayerID();
						LOG(INFO) << "playercount:" << m_playerCount;
						this->exitGame(player->getPlayerID());
					}
				}
				else
				{
					m_accumilatedTime += dt;
					m_world->updateFrame();
					m_lastRunTime = m_accumilatedTime;
				}
			}
		}
		else
		{
			this->update_Run(dt);
		}
	}
		break;
	case GameLogic::RUN_STATE::WAIT:
	{
		this->update_Wait(dt);
	}
		break;
	default:
		break;
	}
}

void GameLogic::update_WaitConnect(float dt)
{
	bool hasOffline = false;
	// 玩家长时间未连接成功则将其踢出游戏,避免游戏一直无法进行
	for (auto i = 0; i < m_playerCount; ++i)
	{
		if (m_players[i]->isOffline())
		{
			hasOffline = true;
			if (m_pApplication->getRunTime() - m_players[i]->getOfflineTime() > MAX_WAIT_CONNECT_TIME)
			{
				LOG(INFO) << "Not connected to the game for a long time, forced exit, playerid: " << m_players[i]->getPlayerID();
				LOG(INFO) << "playercount:" << m_playerCount;
				this->exitGame(m_players[i]->getPlayerID());
				// 必须break
				break;
			}
		}
	}

	// 全部连接成功
	if (!hasOffline)
	{
		m_state = GameLogic::RUN_STATE::READY;
	}
}

void GameLogic::update_Ready(float dt)
{
	// 准备阶段不允许玩家掉线,将离线玩家踢出游戏
	bool exitGameTag = false;
	do 
	{
		exitGameTag = false;
		for (auto i = 0; i < m_playerCount; ++i)
		{
			if (m_players[i]->isOffline())
			{
				exitGameTag = true;
				LOG(INFO) << "Offline in preparation stage, forced exit, playerid: " << m_players[i]->getPlayerID();
				LOG(INFO) << "playercount:" << m_playerCount;
				this->exitGame(m_players[i]->getPlayerID());
				// 必须break
				break;
			}
		}
	} while (exitGameTag);

	if (m_playerCount <= 0)
		return;

	bool finishAllTag = true;
	for (auto i = 0; i < m_playerCount; ++i)
	{
		if (!m_players[i]->getLoadFinish())
		{
			finishAllTag = false;
			break;
		}
	}

	// 全部准备完毕
	if (finishAllTag)
	{
		m_state = GameLogic::RUN_STATE::RUN;

		G_ASSERT(m_world->getGameLogicFrame() == 0);

		// 此处不发送 RunNextFrameAck 消息，避免客户端收到两次此消息且nextframe都是0
		//msg::RunNextFrameAck ack;
		//ack.set_nextframe(0);
		//for (auto i = 0; i < m_playerCount; ++i)
		//{
		//	auto input = ack.mutable_frames()->Add();
		//	input->set_pid(m_players[i]->getPlayerID());
		//	input->set_frame(0);
		//	input->set_key_down(0);
		//}
		//this->sendToAllPlayer(ack.Id, ack);
		// 让下一逻辑帧更新ping值
		m_pingTime = 10000.0f;
	}
}

void GameLogic::update_Run(float dt)
{
	m_accumilatedTime += dt;

	uint32_t maxFrame = 0U;
	uint32_t minFrame = UINT_MAX;
	for (auto i = 0; i < m_playerCount; ++i)
	{
		maxFrame = MAX(maxFrame, m_players[i]->getLastRecvFrame());
		minFrame = MIN(minFrame, m_players[i]->getLastRecvFrame());
	}

	// 目标帧率
	uint32_t targetFrame = maxFrame + MAX_LEAD_FRAME_DIS_MAX_CLIENT;
	// 服务器逻辑以网速最好的客户端逻辑帧步进
	if (targetFrame >= m_world->getGameLogicFrame())
	{
		//
		if (m_runNextFrameAckCache.frames_size() != m_playerCount)
		{
			m_runNextFrameAckCache.clear_frames();
			for (auto i = 0; i < m_playerCount; ++i)
			{
				auto pFrame = m_runNextFrameAckCache.add_frames();
				m_pCacheFrameInputs[i] = pFrame;
			}
		}

		// 将每个玩家本帧输入进行广播
		for (auto i = 0; i < m_playerCount; ++i)
		{
			auto input = m_players[i]->getInput(m_world->getGameLogicFrame());
			if (input)
			{
				m_pCacheFrameInputs[i]->CopyFrom(*input);
			}
			else
			{
				// 使用默认输入
				m_pCacheFrameInputs[i]->set_key_down(0);
			}

			m_pCacheFrameInputs[i]->set_pid(m_players[i]->getPlayerID());
			m_pCacheFrameInputs[i]->set_frame(m_world->getGameLogicFrame());

			// 保存输入历史
			m_pastRecords.add_frames()->CopyFrom(*m_pCacheFrameInputs[i]);
		}

		// 添加玩家离开游戏消息
		if (m_exitPlayers.empty() == false)
		{
			for (auto& it : m_exitPlayers)
			{
				bool ok = false;
				for (auto i = 0; i < m_playerCount; ++i)
				{
					if (m_pCacheFrameInputs[i]->pid() == it)
					{
						ok = true;
						m_pCacheFrameInputs[i]->set_key_down(m_pCacheFrameInputs[i]->key_down() | G_KEY_EXIT_GAME);
						break;
					}
				}

				if (!ok)
				{
					auto pFrame = m_runNextFrameAckCache.add_frames();
					pFrame->set_pid(it);
					pFrame->set_key_down(G_KEY_EXIT_GAME);
				}
			}
			m_exitPlayers.clear();
		}

		m_runNextFrameAckCache.set_nextframe(m_world->getGameLogicFrame());
		sendToAllPlayer(m_runNextFrameAckCache.Id, m_runNextFrameAckCache);

		m_world->updateFrame();

		//printf("do frame... %d\n", m_world->getGameLogicFrame());
		m_lastRunTime = m_accumilatedTime;
	}

	// 多个玩家
	if (m_playerCount > 1)
	{
		// 服务逻辑超过了最慢客户端太多,则等待最慢的客户端一段时间
		if (minFrame < m_world->getGameLogicFrame() && m_world->getGameLogicFrame() - minFrame > MAX_LEAD_FRAME_DIS_MIN_CLIENT)
		{
			m_state = RUN_STATE::WAIT;
			m_waitTime = 0.0f;
		}
	}
	else
	{
		// 只有一个玩家,判断他的离线时间
		auto player = getSlowestPlayer();
		if (player && player->isOffline())
		{
			// 离线太久,踢出游戏
			if (m_pApplication->getRunTime() - player->getOfflineTime() > MAX_OFF_LINE_TIME_SINGLE_PLAYER)
			{
				LOG(INFO) << "Offline too long(2), forced exit, playerid: " << player->getPlayerID();
				LOG(INFO) << "playercount:" << m_playerCount;
				this->exitGame(player->getPlayerID());
			}
		}
	}

	// 太长时间没有跑过逻辑帧,应该是客户端推帧服务器没有收到,结束游戏
	// 这个时间应该大于离线等待时间
	if (m_accumilatedTime - m_lastRunTime > 120.0f)
	{
		m_lastRunTime = m_accumilatedTime;
		// 全部踢出游戏
		for (auto i = m_playerCount - 1; i >= 0; --i)
		{
			LOG(INFO) << "Client push frame server did not receive, end the game ";
			this->exitGame(m_players[i]->getPlayerID());
		}
	}

	
	// 清除比较久的操作记录
	if (m_pastRecords.frames_size() > MAX_RECORDS_COUNT * 2)
	{
		// 超过最多保留帧数多少之后开始清理
		const int32_t stepCount = 50;

		auto curFrame = m_pastRecords.frames().begin()->frame();
		if (m_world->getGameLogicFrame() - curFrame > MAX_RECORDS_COUNT + stepCount)
		{
			int index = 0;
			do
			{
				if (index >= m_pastRecords.frames_size())
					break;

				curFrame = m_pastRecords.frames().Get(index).frame();
				if (m_world->getGameLogicFrame() - curFrame <= MAX_RECORDS_COUNT)
					break;

				index++;
			} while (true);

			if (index > 0)
				m_pastRecords.mutable_frames()->DeleteSubrange(0, index);

		}
	}
}

void GameLogic::update_Wait(float dt)
{
	uint32_t maxFrame = 0U;
	uint32_t minFrame = UINT_MAX;
	for (auto i = 0; i < m_playerCount; ++i)
	{
		maxFrame = MAX(maxFrame, m_players[i]->getLastRecvFrame());
		minFrame = MIN(minFrame, m_players[i]->getLastRecvFrame());
	}

	m_waitTime += dt;
	if (minFrame >= m_world->getGameLogicFrame())
	{
		m_state = RUN_STATE::RUN;
		m_waitTime = 0.0f;
	}
	// 等待时间太久,将该玩家踢出游戏
	if (m_waitTime >= MAX_OFF_LINE_TIME_MULTIPLE_PLAYER)
	{
		auto player = getSlowestPlayer();
		if (player)
		{
			LOG(INFO) << "Waiting time is too long, forced exit, playerid: " << player->getPlayerID();
			LOG(INFO) << "playercount:" << m_playerCount;
			this->exitGame(player->getPlayerID());
		}
	}
}

void GameLogic::pingUpdate(float dt)
{
	m_pingTime += dt;
	m_pingPushTime += dt;

	// 采集ping值
	if (m_pingTime >= GET_PING_INTERVAL)
	{
		m_pingTime = 0.0f;

		msg::Ping req;
		req.set_timestamp(GApplication::getInstance()->getRunTime32());
		for (auto i = 0; i < m_playerCount; ++i)
		{
			req.set_ping(m_players[i]->getPing());
			SEND_PB_MSG(m_pNetService, m_players[i]->getSessionID(), req);
		}
	}

	// 推送ping值
	if (m_pingPushTime >= PUSH_PING_INTERVAL)
	{
		m_pingPushTime = 0.0f;

		msg::PushPingInfo ntf;
		for (auto i = 0; i < m_playerCount; ++i)
		{
			auto info = ntf.add_infos();
			info->set_pid(m_players[i]->getPlayerID());
			info->set_ping(m_players[i]->getPing());
		}
		sendToAllPlayer(ntf.Id, ntf);
	}
}

//////////////////////////////////////////////////////////////////////////

uint32_t GameLogic::getGameLogicFrame() const
{
	if (m_world)
		return m_world->getGameLogicFrame();
	return 0;
}

int32_t GameLogic::getGameStatus() const
{
	return (int32_t)m_state;
}

////////////////////////////////////////////////////////////////////////////////////////////

err::Code GameLogic::joinCode(uint32_t sessionID, const msg::JoinFightReq& req)
{
	// 玩家已经被踢出游戏
	if (m_playerIDSet.count(req.playerid()) == 0)
	{
		return err::Code::FIGHT_LEAVE_GAME;
	}

	err::Code code = err::Code::UNKNOWN;
	for (auto i = 0; i < m_playerCount; ++i)
	{
		if (m_players[i] && m_players[i]->getPlayerID() == req.playerid())
		{
			//// 此玩家已经在游戏中了
			//if (m_players[i]->getSessionID() != GamePlayer::INVALID_SESSION_ID)
			//{
			//	code = err::Code::FIGHT_PLAYING;
			//	break;
			//}
			code = err::Code::SUCCESS;
		}
	}

	if (code == err::Code::SUCCESS)
	{
		if (m_state == GameLogic::RUN_STATE::READY || m_state == GameLogic::RUN_STATE::WAIT_CONNECT)
		{
			// 不为0有问题
			if (req.frame() != 0)
			{
				code = err::Code::FIGHT_FRAME_ERR;
			}
		}
		else
		{
			// 离线模式加入
			if (m_initArgs.isOfflineMode)
			{
				code = err::Code::SUCCESS;
			}
			else
			{
				if (m_pastRecords.frames_size() <= 0)
				{
					// 客户端的帧数和服务器帧数不一致
					if (req.frame() > 0)
					{
						code = err::Code::FIGHT_FRAME_ERR;
					}
				}
				else
				{
					if (m_pastRecords.frames().Get(0).frame() > req.frame())
					{
						// 服务器缓存记录不足以支持客户端断线重连
						code = err::Code::FIGHT_PAST_RECORDS_INC;
					}
				}
			}
		}
	}

	if (code != err::Code::UNKNOWN)
		return code;

	// 找不到该玩家
	return err::Code::FIGHT_NOE_FOUND_PLAYER;
}

void GameLogic::doJoin(uint32_t sessionID, const msg::JoinFightReq& req)
{
	// 设置sessionID
	for (auto i = 0; i < m_playerCount; ++i)
	{
		if (m_players[i] && m_players[i]->getPlayerID() == req.playerid())
		{
			m_players[i]->setSessionID(sessionID);
		}
	}

	//msg::PlayerReadyNotify ntf;
	//ntf.set_pid(req.playerid());
	//this->sendToAllPlayer(ntf.Id, ntf);

	if (m_state == GameLogic::RUN_STATE::READY || m_state == GameLogic::RUN_STATE::WAIT_CONNECT)
	{
		sendLoadingPercentToAllPlayer();
	}
	else
	{
		pushFrameInfo(req.frame(), sessionID);
	}
}

void GameLogic::exitGame(int64_t playerID)
{
	if (!containPlayer(playerID))
		return;

	m_exitPlayers.push_back(playerID);

	std::unique_ptr<GamePlayer> tmp[G_FIGHT_MAX_PLAYER_COUNT];
	int32_t index = 0;
	for (auto i = 0; i < G_FIGHT_MAX_PLAYER_COUNT; ++i)
	{
		if (m_players[i] && m_players[i]->getPlayerID() != playerID)
		{
			tmp[index].reset(m_players[i].release());
			index++;
		}
		m_players[i] = NULL;
	}

	for (auto i = 0; i < index; ++i)
	{
		m_players[i].reset(tmp[i].release());
	}

	m_playerCount--;

	G_ASSERT(index == m_playerCount);
}

err::Code GameLogic::exitGameWithSessionID(uint32_t sessionID)
{
	for (auto i = 0; i < m_playerCount; ++i)
	{
		if (m_players[i]->getSessionID() == sessionID)
		{
			LOG(INFO) << "exitGameWithSessionID, playerid: " << m_players[i]->getPlayerID();
			LOG(INFO) << "playercount:" << m_playerCount;
			this->exitGame(m_players[i]->getPlayerID());
			return err::Code::SUCCESS;
		}
	}
	return err::Code::FIGHT_LEAVE_GAME;
}

std::vector<int64_t> GameLogic::getPlayerIds()
{
	std::vector<int64_t> ids;

	for (auto it = m_playerIDSet.begin(); it != m_playerIDSet.end(); ++it)
	{
		ids.push_back(*it);
	}
	return ids;
}

bool GameLogic::containPlayer(int64_t playerID)
{
	for (auto i = 0; i < m_playerCount; ++i)
	{
		if (m_players[i]->getPlayerID() == playerID)
			return true;
	}
	return false;
}


void GameLogic::sendToAllPlayer(int32_t msgID, const ::google::protobuf::MessageLite& msg)
{
	for (auto i = 0; i < m_playerCount; ++i)
	{
		SEND_PB_MSG_EX(m_pNetService, m_players[i]->getSessionID(), msgID, msg);
	}
}

GamePlayer* GameLogic::getPlayerBySessionID(uint32_t sessionID)
{
	for (auto i = 0; i < m_playerCount; ++i)
	{
		if (m_players[i]->getSessionID() == sessionID)
			return m_players[i].get();
	}
	return NULL;
}

// 获取逻辑帧最慢的一个玩家
GamePlayer* GameLogic::getSlowestPlayer()
{
	GamePlayer* player = NULL;
	uint32_t minFrame = UINT_MAX;
	for (auto i = 0; i < m_playerCount; ++i)
	{
		if (m_players[i]->getLastRecvFrame() < minFrame)
		{
			minFrame = m_players[i]->getLastRecvFrame();
			player = m_players[i].get();
		}
	}
	return player;
}

void GameLogic::sendLoadingPercentToAllPlayer()
{
	msg::PlayerLoadingAck ack;

	bool finish = true;
	for (auto i = 0; i < m_playerCount; ++i)
	{
		ack.add_percent(m_players[i]->getLoadPercent());
		ack.add_pid(m_players[i]->getPlayerID());

		if (!m_players[i]->getLoadFinish())
		{
			finish = false;
		}
	}
	ack.set_finish(finish);
	sendToAllPlayer(ack.Id, ack);
}

// 向玩家推帧
void GameLogic::pushFrameInfo(uint32_t startFrame, uint32_t sessionID)
{
	// MSG_PUSH_FRAME_BEGIN
	msg::PushFrameInputBegin begin;
	SEND_PB_MSG(m_pNetService, sessionID, begin);


	msg::PushFrameInput ack;

	int32_t curCount = 0;
	bool sendTag = false;
	uint32_t frame = 0;

	for (auto i = 0; i < m_pastRecords.frames_size(); ++i)
	{
		auto& data = m_pastRecords.frames().Get(i);
		if (data.frame() >= startFrame)
		{
			sendTag = true;
			curCount++;

			if (frame != data.frame())
			{
				frame = data.frame();
				curCount++;
				// 大于一定帧数后进行分片推帧
				if (curCount > 50)
				{
					SEND_PB_MSG(m_pNetService, sessionID, ack);
					ack.clear_frames();
					sendTag = false;
				}
			}
			ack.set_lastframe(data.frame());
			ack.add_frames()->CopyFrom(data);
		}
	}

	if (sendTag)
	{
		SEND_PB_MSG(m_pNetService, sessionID, ack);
	}

	// MSG_PUSH_FRAME_END
	msg::PushFrameInputEnd end;
	SEND_PB_MSG(m_pNetService, sessionID, end);
}

void GameLogic::onMsg_RunNextFrameReq(uint32_t sessionID, const msg::RunNextFrameReq& req)
{
	auto player = getPlayerBySessionID(sessionID);
	if (player == NULL)
	{
		m_pNetService->disconnect(sessionID);
		return;
	}

	// 只有在等待和运行阶段接收玩家输入
	if (m_state != GameLogic::RUN_STATE::WAIT && m_state != GameLogic::RUN_STATE::RUN)
	{
		return;
	}

	auto lastRecvFrame = player->getLastRecvFrame();
	// 客户端向服务器发送以往数据？,超过阈值视为作弊，故意拖延游戏节奏
	if (lastRecvFrame > req.frame() && lastRecvFrame - req.frame() > (MAX_LEAD_FRAME_DIS_MAX_CLIENT + 3))
	{
		LOG(INFO) << "recv frame: " << req.frame() << ", last recv frame: " << lastRecvFrame;
		LOG(INFO) << "Deceiving the server to slow down the game, forced exit, playerid: " << player->getPlayerID();
		LOG(INFO) << "playercount:" << m_playerCount;
		this->exitGame(player->getPlayerID());
		return;
	}

	auto curFrame = m_world->getGameLogicFrame();
	if (req.frame() > curFrame + 1)
	{
		// 客户端逻辑帧超过服务端逻辑帧,视为客户端作弊
		// 当前消息视为无效消息
		LOG(INFO) << "recv invalid input, forced exit, playerid: " << player->getPlayerID();
		LOG(INFO) << "playercount:" << m_playerCount;
		this->exitGame(player->getPlayerID());
		return;
	}

	if (m_state == GameLogic::RUN_STATE::WAIT)
	{
		// 等待阶段不接收玩家输入
		player->setLastRecvFrame(MAX(lastRecvFrame, req.frame()));
	}
	else // RUN
	{
		player->setLastRecvFrame(MAX(lastRecvFrame, req.frame()));
		// 客户端发送的操作数据距离当前逻辑帧太久,直接抛弃操作
		if (curFrame - req.frame() > 100)
			return;

		player->input(req, curFrame);
	}
}

void GameLogic::onMsg_PlayerLoadingReq(uint32_t sessionID, const msg::PlayerLoadingReq& req)
{
	auto player = getPlayerBySessionID(sessionID);
	if (player == NULL)
	{
		m_pNetService->disconnect(sessionID);
		return;
	}

	if (player->getLoadFinish())
		return;

	player->setLoadPercent(MIN(req.percent(), 1.0f));
	player->setLoadFinish(req.finish());

	if (player->getLoadFinish())
		player->setLoadPercent(1.0f);

	this->sendLoadingPercentToAllPlayer();
}

void GameLogic::onMsg_Pong(uint32_t sessionID, const msg::Pong& req)
{
	auto player = getPlayerBySessionID(sessionID);
	if (player == NULL)
	{
		m_pNetService->disconnect(sessionID);
		return;
	}

	auto cur = GApplication::getInstance()->getRunTime32();
	auto diff = cur - req.timestamp();

	if (diff < 0)
	{
		LOG(WARNING) << "error ping response, diff:" << diff << "playerid:" << player->getPlayerID();
		diff = 0;
	}
	player->setPing(diff);
}
