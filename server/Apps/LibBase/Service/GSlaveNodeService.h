#pragma once

#include "GIService.h"

/// ������ͬ��-�ӽڵ����
class GSlaveNodeService : public GIService
{
public:

	G_DEFINE_SERVICE(GSlaveNodeService);

	virtual bool onInit() override;

	virtual void onStartService() override;

	virtual void onStopService() override;

	virtual void onUpdate(float) override;

	virtual void onDestroy() override;

private:
	std::unique_ptr<net_uv::TCPClient> m_client;
};
