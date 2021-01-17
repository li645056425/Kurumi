#pragma once

#include "ecs/anaxHelper.h"
#include "foundation/animator/GAnimData.h"

enum kArmaturePlayMode
{
	ONCE = 0,
	LOOP
};

enum class kArmatureRenderAction
{
	NONE,
	PAUSE,		// ��ͣ
	PURSUE,		// ׷���߼���Ⱦ֡
	AWAIT,		// �ȴ��߼���Ⱦ֡
	RUN,		// ����ִ��
};


// �����������
class ArmatureComponent : public BaseComponent
{
public:
	ArmatureComponent() 
	{
		curFrameIndex = 0;
		cmdIndex = 0;
		cmdCount = 0;
		mode = kArmaturePlayMode::ONCE;
		playing = false;
	}
	// ��ǰ֡
	uint32_t curFrameIndex;
	// ��ǰ����������
	std::string curAniName;
	// �����ļ�,���ڻ�ȡ�����ļ�����
	//std::string aniFile;
	// ��ǰ��������
	std::string curAniCMD;
	// ��ɫ��,���ڻ�ȡ��������
	std::string roleName;

	int32_t cmdIndex;
	int32_t cmdCount;

	kArmaturePlayMode mode;
	bool playing;
};

// ����������Ⱦ���
class ArmatureRenderComponent : public BaseComponent
{
public:
	ArmatureRenderComponent()
	{
#if G_TARGET_CLIENT
		actionType = kArmatureRenderAction::NONE;
		render = NULL;
#endif
	}

#if G_TARGET_CLIENT
	cocostudio::Armature* render;
	kArmatureRenderAction actionType;
	std::string m_cacheRoleName;
#endif
};



//
//// ����������ײ���
//class ArmatureCollisionComponent : public anax::Component, public Ref
//{
//public:
//	ArmatureCollisionComponent()
//		: m_defCacheDirty(true)
//		, m_attCacheDirty(true)
//	{
//		m_defRectCache.reserve(5);
//		m_attRectCache.reserve(5);
//	}
//	virtual ~ArmatureCollisionComponent() {}
//
//	// ��Ϊ��������ײ�ص�
//	LuaFunction m_attCollisionCall;
//	// ��Ϊ�ܻ�����ײ�ص�
//	LuaFunction m_defCollisionCall;
//
//	bool m_defCacheDirty;
//	bool m_attCacheDirty;
//
//	// ��ײ���λ���
//	std::vector<ArmatureCollisionRect> m_defRectCache;
//	std::vector<ArmatureCollisionRect> m_attRectCache;
//};
//
//// ����������ײ�������
//class CollisionFilterComponent : public anax::Component, public Ref
//{
//public:
//	CollisionFilterComponent() 
//	{
//		m_groupIndex = 0;
//		m_collisionEnable = false;
//	}
//	virtual ~CollisionFilterComponent() {}
//	
//	int m_groupIndex;
//	bool m_collisionEnable;
//};


