#pragma once

#include "GIService.h"

/// ������ͬ��-���ڵ����
class GMasterNodeService : public GIService
{
public:
	G_DEFINE_SERVICE(GMasterNodeService);

	virtual bool onInit() override;

	virtual void onStartService() override;

	virtual void onStopService() override;

	virtual void onUpdate(float) override;

	virtual void onDestroy() override;
	
private:
	std::unique_ptr<net_uv::TCPServer> m_svr;
};
