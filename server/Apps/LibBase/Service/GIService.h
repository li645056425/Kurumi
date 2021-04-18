#pragma once

#include "GClassTypeId.h"
#include "GPlatformMacros.h"

// ���������ɹ�
static const uint32_t SCODE_START_SUCCESS = 0;
// ��������ʧ��,�Ҵ˷���Ӱ��ϵͳ��������,ϵͳ�������ʧ����Ϣ������
static const uint32_t SCODE_START_FAIL_RUN = 1;
// ��������ʧ��,ϵͳ���в���Ҫ�˷���,�������
static const uint32_t SCODE_START_FAIL_NO_ERR = 2;
// ��������ʧ��,�Ҵ˷���Ϊϵͳ��Ҫ����,�����˳�APP
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

	virtual void onStartService();

	virtual void onStopService();

	virtual void onUpdate(float);

	virtual void onDestroy();

public:

	virtual void stopService(const std::function<void()>& call = NULL);

	G_FORCEINLINE void setEnableUpdate(bool value);

	G_FORCEINLINE bool isRunning();

	G_FORCEINLINE bool isStop();

	G_FORCEINLINE bool isEnableUpdate();

	virtual const char* serviceName() const;

protected:

	void stopServiceFinish();
	
protected:
	friend class GServiceMgr;
	GServiceStatus m_status;
	GServiceMgr* m_serviceMgr;
	std::function<void()> m_stopCallback;
	bool m_enableUpdate;
};


bool GIService::isRunning()
{
	return m_status == GServiceStatus::RUNNING;
}

bool GIService::isStop()
{
	return m_status == GServiceStatus::STOP;
}

bool GIService::isEnableUpdate()
{
	return m_enableUpdate;
}

void GIService::setEnableUpdate(bool value)
{
	m_enableUpdate = value;
}


/////////////////////////////////////////////////////////////////////////////////

template<class T>
detail::GTypeId GServiceTypeId()
{
	return detail::GClassTypeId<GIService>::GetTypeId<T>();
}

#define G_DEFINE_SERVICE(SNAME) const char* serviceName() const override { return #SNAME; }
#define G_ERROR_MISS_SERVICE(service) LOG(ERROR) << "[" << this->serviceName() << "]" << " Missing dependent service [" << #service << "]";
#define G_CHECK_SERVICE(service) if(m_serviceMgr->getService<service>() == NULL) { G_ERROR_MISS_SERVICE(service); return SCODE_START_FAIL_EXIT_APP; }
