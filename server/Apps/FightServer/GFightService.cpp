﻿#include "GFightService.h"

// 最大同时战斗数量
static int32_t MAX_FIGHT_COUNT = 100;

// 游戏逻辑帧率
static float LOGIC_INTERVAL = 1 / 30.0f;

uint32_t GFightService::onInit()
{
	// 依赖网络服务,用于与玩家通信
	G_CHECK_SERVICE(GNetService);
	// 依赖从节点同步服务,与主节点通信
	G_CHECK_SERVICE(GSlaveNodeService);
	// 配置服务,读取战斗服务相关配置
	G_CHECK_SERVICE(GConfigService);

	m_pNetService = m_serviceMgr->getService<GNetService>();
	m_pSlaveNode = m_serviceMgr->getService<GSlaveNodeService>();
	auto pConfigService = m_serviceMgr->getService<GConfigService>();

	// 配置读取
	auto appName = GApplication::getInstance()->getAppName();
	MAX_FIGHT_COUNT = pConfigService->iniReader().GetInteger(appName, "MaxFightCount", 100);
	
	//! 服务器通信
	ON_PB_MSG_CLASS_CALL(m_pSlaveNode->noticeCenter(), svr_msg::NewFightReq, onMsg_NewFightReq);

	//! 客户端通信
	ON_PB_MSG_CLASS_CALL(m_pNetService->noticeCenter(), msg::JoinFightReq, onMsg_JoinFightReq);
	ON_PB_MSG_CLASS_CALL(m_pNetService->noticeCenter(), msg::ExitFightReq, onMsg_ExitFightReq);

	// 随机数生成器
	uint32_t seed = time(NULL);
	m_random = std::make_unique<GRandom>(seed, seed + 1);

	m_arrFightInfo.reserve(20);
	return SCODE_START_SUCCESS;
}

void GFightService::onStartService()
{
	// 定时清理已完成的战斗
	GApplication::getInstance()->getScheduler()->schedule([=](float) {
		this->clearInvalid();
	}, this, 5.0f, false, "check");

	GApplication::getInstance()->getScheduler()->schedule([=](float) {
		this->logicUpdate(LOGIC_INTERVAL);
	}, this, LOGIC_INTERVAL, false, "logicUpdate");
}

void GFightService::onStopService()
{
	this->stopServiceFinish();
	m_pSlaveNode->noticeCenter()->delListener(this);
	m_pNetService->noticeCenter()->delListener(this);
}

void GFightService::logicUpdate(float dt)
{
	for (auto& it : m_arrFightInfo)
	{
		if (it->getIsFinish() == false)
		{
			it->update(dt);
		}
	}
}

void GFightService::onDestroy()
{
	for (auto it : m_arrFightInfo)
	{
		delete it;
	}
	m_arrFightInfo.clear();
}

// 销毁世界
void GFightService::destroyFight(int32_t uuid)
{
	for (auto it = m_arrFightInfo.begin(); it != m_arrFightInfo.end(); ++it)
	{
		if ((*it)->getuuid() == uuid)
		{
			delete *it;
			m_arrFightInfo.erase(it);
			break;
		}
	}
}

// 统计当前正在战斗的数量
int32_t GFightService::curFightCount()
{
	int32_t count = 0;
	for (auto& it : m_arrFightInfo)
	{
		if (it->getIsFinish() == false)
			count++;
	}
	return count;
}

// 清除无效战斗
void GFightService::clearInvalid()
{
	for (auto it = m_arrFightInfo.begin(); it != m_arrFightInfo.end(); )
	{
		if ((*it)->getIsFinish())
		{
			delete *it;
			it = m_arrFightInfo.erase(it);
		}
		else
		{
			++it;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
/// MSG
/////////////////////////////////////////////////////////////////////////////////////////////
void GFightService::onMsg_NewFightReq(uint32_t sessionID, const svr_msg::NewFightReq& msg)
{
	svr_msg::NewFightAck ack;
	ack.set_code(err::Code::UNKNOWN);
	ack.set_uuid(0);
	ack.set_tag(msg.tag());
	ack.set_fighttype(msg.fighttype());
	ack.set_nettype((int32_t)m_pNetService->getNetType());

	// 参数错误
	if (msg.roles_size() <= 0)
	{
		ack.set_code(err::Code::PARAM_ERROR);
		SEND_PB_MSG_NO_SESSION(m_pSlaveNode, ack);
		return;
	}

	// 服务器过载
	if (this->curFightCount() >= MAX_FIGHT_COUNT)
	{
		ack.set_code(err::Code::OVERLOAD);
		SEND_PB_MSG_NO_SESSION(m_pSlaveNode, ack);
		return;
	}

	auto logic = new(std::nothrow) GameLogic();
	// 内存不足
	if (logic == NULL)
	{
		ack.set_code(err::Code::NO_MEMORY);
		SEND_PB_MSG_NO_SESSION(m_pSlaveNode, ack);
		return;
	}

	// 初始化参数
	GGameWorldInitArgs args;
	args.mapId = msg.mapid();
	args.randomSeed = (uint32_t)time(NULL) + m_random->random(1, 10000);
	args.uuidSeed = m_random->random(1, 10000);
	args.isOfflineMode = msg.isofflinemode();
	
	if (msg.roles_size() > 1)
	{
		args.isOfflineMode = false;
	}


	// 初始化失败
	auto code = logic->init(args, msg.roles());
	if (code != err::Code::SUCCESS)
	{
		delete logic;
		ack.set_code(code);
		SEND_PB_MSG_NO_SESSION(m_pSlaveNode, ack);
		return;
	}
	m_arrFightInfo.push_back(logic);

	ack.set_code(err::Code::SUCCESS);
	ack.set_uuid(logic->getuuid());
	SEND_PB_MSG_NO_SESSION(m_pSlaveNode, ack);
}


void GFightService::onMsg_JoinFightReq(uint32_t sessionID, const msg::JoinFightReq& msg)
{
	msg::JoinFightAck ack;
	ack.set_code(err::Code::UNKNOWN);

	for (auto& it : m_arrFightInfo)
	{
		if (it->getuuid() == msg.fightuuid() && it->getIsFinish() == false)
		{
			auto pWorldinfo = ack.mutable_worldinfo();
			pWorldinfo->set_svr_status(it->getGameStatus());
			pWorldinfo->set_frame(it->getGameLogicFrame());
			pWorldinfo->set_mapid(it->getInitArgs().mapId);
			pWorldinfo->set_randomseed(it->getInitArgs().randomSeed);
			pWorldinfo->set_uuidseed(it->getInitArgs().uuidSeed);
			pWorldinfo->set_isofflinemode(it->getInitArgs().isOfflineMode);

			auto ids = it->getPlayerIds();
			for (auto& id : ids)
			{
				pWorldinfo->add_roles(id);
			}

			ack.set_code(it->joinCode(sessionID, msg));
			SEND_PB_MSG(m_pNetService, sessionID, ack);

			if (ack.code() == err::Code::SUCCESS)
			{
				it->doJoin(sessionID, msg);
			}
			return;
		}
	}

	// 找不到对应ID
	ack.set_code(err::Code::NOT_FOUND_FIGHT);
	SEND_PB_MSG(m_pNetService, sessionID, ack);
}

void GFightService::onMsg_ExitFightReq(uint32_t sessionID, const msg::ExitFightReq& msg)
{
	msg::ExitFightAck ack;
	ack.set_code(err::Code::NOT_FOUND_FIGHT);

	for (auto& it : m_arrFightInfo)
	{
		if (it->getuuid() == msg.fightuuid())
		{
			ack.set_code(it->exitGameWithSessionID(sessionID));
			break;
		}
	}

	SEND_PB_MSG(m_pNetService, sessionID, ack);
}
