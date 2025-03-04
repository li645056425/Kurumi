﻿#include "GLoginService.h"
#include "GAccountMgrService.h"

enum class LOGIN_PLATFORM
{
	Android,
	Ios,
	Windows,
	Unknown
};

inline LOGIN_PLATFORM getLoginPlatform(const std::string& platform)
{
	if (strcmpi(platform.c_str(), "Android") == 0)
		return LOGIN_PLATFORM::Android;
	if (strcmpi(platform.c_str(), "Ios") == 0)
		return LOGIN_PLATFORM::Ios;
	if (strcmpi(platform.c_str(), "Windows") == 0)
		return LOGIN_PLATFORM::Windows;
	return LOGIN_PLATFORM::Unknown;
}

// 模板.cpp

uint32_t GLoginService::onInit()
{
	G_CHECK_SERVICE(GHttpService);
	G_CHECK_SERVICE(GAccountMgrService);
	G_CHECK_SERVICE(GMasterNodeService);

	auto scheduler = GApplication::getInstance()->getScheduler();
	scheduler->schedule([=](float){
		this->clearInvalidToken();
	}, this, 60.0f, false, "check_token");

	auto now = (unsigned int)::time(NULL);
	m_random = std::unique_ptr<GRandom>(new  GRandom(now, now + 1));

	init_Http();
	init_MasterNode();

	return SCODE_START_SUCCESS;
}

void GLoginService::init_Http()
{
	auto httpSvice = m_serviceMgr->getService<GHttpService>();

	httpSvice->getPathNoticeCenter()->addListener(this, "/test", [](const net_uv::HttpRequest& request, net_uv::HttpResponse* response)
	{
		response->setStatusCode(net_uv::HttpResponse::k200Ok);

		std::string content;

		content.append(StringUtils::format("method : %s\n", request.methodString()));
		content.append(StringUtils::format("query : %s\n", request.query().c_str()));
		content.append(StringUtils::format("path : %s\n", request.path().c_str()));

		content.append("header:\n");
		for (auto& it : request.headers())
		{
			content.append(StringUtils::format("\t%s : %s\n", it.field.c_str(), it.value.c_str()));
		}

		content.append("params:\n");
		for (auto& it : request.params())
		{
			content.append(StringUtils::format("\t%s : %s\n", it.first.c_str(), it.second.c_str()));
		}

		response->setBody(content);
	});

	httpSvice->noticeCenter(HTTP_GET)->addListener(this, "/api/login", [=](const net_uv::HttpRequest& request, net_uv::HttpResponse* response)
	{
		response->setStatusCode(net_uv::HttpResponse::k200Ok);
		this->onHttpRequest_Login(request, response);
	});

	httpSvice->noticeCenter(HTTP_GET)->addListener(this, "/api/register", [=](const net_uv::HttpRequest& request, net_uv::HttpResponse* response)
	{
		response->setStatusCode(net_uv::HttpResponse::k200Ok);
		this->onHttpRequest_Register(request, response);
	});

	httpSvice->noticeCenter(HTTP_GET)->addListener(this, "/api/svrlist", [=](const net_uv::HttpRequest& request, net_uv::HttpResponse* response)
	{
		response->setStatusCode(net_uv::HttpResponse::k200Ok);
		this->onHttpRequest_ServerList(request, response);
	});
}

void GLoginService::init_MasterNode()
{
	m_pMasterNodeService = m_serviceMgr->getService<GMasterNodeService>();

	
	ON_PB_MSG_CLASS_CALL(m_pMasterNodeService->noticeCenter(), svr_msg::CheckTokenReq, onMsg_CheckTokenReq);
}

void GLoginService::onStopService()
{
	this->stopServiceFinish();

	m_serviceMgr->getService<GHttpService>()->delListener(this);
	m_serviceMgr->getService<GMasterNodeService>()->noticeCenter()->delListener(this);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


void GLoginService::clearInvalidToken()
{
	if (m_tokenMap.empty())
		return;

	auto now = ::time(NULL);
	for (auto it = m_tokenMap.begin(); it != m_tokenMap.end(); )
	{
		if (now - it->second.time > 60 * 60 * 2)
		{
			it = m_tokenMap.erase(it);
		}
		else
		{
			++it;
		}
	}
}

err::Code GLoginService::checkToken(const std::string& playerId, const std::string token)
{
	auto it = m_tokenMap.find(playerId);
	if (it == m_tokenMap.end())
		return err::Code::NOT_FOUND;

	it->second.time = ::time(NULL);

	if (it->second.token == token)
		return err::Code::SUCCESS;

	return err::Code::OVERDUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

void GLoginService::onHttpRequest_Login(const net_uv::HttpRequest& request, net_uv::HttpResponse* response)
{
	std::string user = request.getParam("user", "");
	std::string pwd = request.getParam("pwd", "");
	std::string platform = request.getParam("platform", "");

	if (!user.empty() && !pwd.empty())
	{
		auto ePlatform = getLoginPlatform(platform);
		if (ePlatform == LOGIN_PLATFORM::Unknown)
		{
			response->setBody("{\"code\":-2,\"msg\":\"Unknown platform\"}");
			return;
		}

		std::string account;
		auto code = m_serviceMgr->getService<GAccountMgrService>()->getAccount(user, pwd, (int32_t)ePlatform, account);
			
		if (code == err::Code::SUCCESS)
		{
			std::string tmp = StringUtils::format("%s-%s-%d", account.c_str(), pwd.c_str(), m_random->random(1, 10000));
			auto token = StringUtils::format("%08X-%08X", m_random->random(1, 10000) + m_tokenMap.size(), NFrame::CRC32(tmp));

			auto it = m_tokenMap.find(account);
			if (m_tokenMap.end() == it)
			{
				TokenInfo info;
				info.token = token;
				info.time = ::time(NULL);
				m_tokenMap[account] = info;
			}
			else
			{
				it->second.token = token;
				it->second.time = ::time(NULL);
			}

			std::string body = StringUtils::format("{\"code\":0,\"msg\":\"ok\",\"token\":\"%s\",\"playerid\":\"%s\"}", token.c_str(), account.c_str());
			response->setBody(body);

			const auto& slaveNodes = m_pMasterNodeService->arrSlaveNodInfos();
			if (slaveNodes.empty() == false)
			{
				// 通知游戏服token发生变化
				svr_msg::TokenChangeNtf ntf;
				ntf.set_account(account);
				ntf.set_token(token);
				for (const auto& it : slaveNodes)
				{
					SEND_PB_MSG(m_pMasterNodeService, it.sessionID, ntf);
				}
			}
		}
		else
		{
			response->setBody(StringUtils::format("{\"code\":%d,\"msg\":\"error\"}", code));
		}
		return;
	}

	response->setBody("{\"code\":-1,\"msg\":\"bad parameter\"}");
}

void GLoginService::onHttpRequest_Register(const net_uv::HttpRequest& request, net_uv::HttpResponse* response)
{
	std::string user = request.getParam("user", "");
	std::string pwd = request.getParam("pwd", "");
	std::string platform = request.getParam("platform", "");
	if (!user.empty() && !pwd.empty())
	{
		auto ePlatform = getLoginPlatform(platform);
		if (ePlatform == LOGIN_PLATFORM::Unknown)
		{
			response->setBody("{\"code\":-2,\"msg\":\"Unknown platform\"}");
			return;
		}

		auto code = m_serviceMgr->getService<GAccountMgrService>()->createAccount(user, pwd, (int32_t)ePlatform);
		if (code == 0)
		{
			response->setBody("{\"code\":0,\"msg\":\"ok\"}");
		}
		else
		{
			response->setBody(StringUtils::format("{\"code\":%d,\"msg\":\"create failed\"}", code));
		}
		return;
	}

	response->setBody("{\"code\":-1,\"msg\":\"bad parameter\"}");
}

void GLoginService::onHttpRequest_ServerList(const net_uv::HttpRequest& request, net_uv::HttpResponse* response)
{
	rapidjson::StringBuffer buf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

	writer.StartObject();
	writer.Key("code");
	writer.Int(0);

	writer.Key("list");

	writer.StartArray();
	for (auto& it : m_serviceMgr->getService<GMasterNodeService>()->arrSlaveNodInfos())
	{
		writer.String(it.info.c_str());
	}
	writer.EndArray();
	
	writer.EndObject();

	response->setBody(buf.GetString());
}


/////////////////////////////////////////////////////////////////////////////////////////////
/// MSG
/////////////////////////////////////////////////////////////////////////////////////////////

void GLoginService::onMsg_CheckTokenReq(uint32_t sessionID, const svr_msg::CheckTokenReq& msg)
{
	auto code = this->checkToken(msg.account(), msg.token());

	svr_msg::CheckTokenAck ack;
	ack.set_code(code);
	ack.set_pid(msg.pid());
	ack.set_session(msg.session());
	ack.set_account(msg.account());
	ack.set_token(msg.token());
	SEND_PB_MSG(m_pMasterNodeService, sessionID, ack);
}

