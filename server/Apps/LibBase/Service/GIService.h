#pragma once

#include "GClassTypeId.h"
#include "GPlatformMacros.h"

// ���������ɹ�
static const uint32_t SCODE_START_SUCCESS = 0;
// ��������ʧ��,�Ҵ˷���Ӱ��ϵͳ��������,ϵͳ�������ʧ����Ϣ������
static const uint32_t SCODE_START_FAIL_RUN = 1;
// ��������ʧ��,ϵͳ���в���Ҫ�˷���,�������
static const uint32_t SCODE_START_FAIL_NO_ERR = 2;
// ��������ʧ��,�Ҵ˷���Ϊϵͳ��Ҫ����,�����Ƴ�APP
static const uint32_t SCODE_START_FAIL_EXIT_APP = 3;

class GIService
{
public:

	enum class GServiceStatus
	{
		RUNNING,
		STOP_ING,
		STOP,
	};

	GIService();

	virtual ~GIService();

public:

	virtual uint32_t onInit() = 0;

	virtual void onStartService() = 0;

	virtual void onStopService() = 0;

	virtual void onUpdate(float) = 0;

	virtual void onDestroy() = 0;

public:

	virtual void stopService(const std::function<void()>& call = NULL);

	G_FORCEINLINE bool isRunning();

	G_FORCEINLINE bool isStop();

	virtual const char* serviceName() const;
	
protected:
	friend class GServiceMgr;
	GServiceStatus m_status;
	GServiceMgr* m_serviceMgr;
	std::function<void()> m_stopCallback;
};


bool GIService::isRunning()
{
	return m_status == GServiceStatus::RUNNING;
}

bool GIService::isStop()
{
	return m_status == GServiceStatus::STOP;
}


/////////////////////////////////////////////////////////////////////////////////

template<class T>
detail::GTypeId GServiceTypeId()
{
	return detail::GClassTypeId<GIService>::GetTypeId<T>();
}

#define G_DEFINE_SERVICE(SNAME) const char* serviceName() const override { return #SNAME; }