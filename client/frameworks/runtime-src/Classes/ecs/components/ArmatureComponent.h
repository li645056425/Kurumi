#pragma once

#include "ecs/anaxHelper.h"
#include "foundation/animator/GAnimData.h"

enum kArmaturePlayMode
{
	ONCE = 0,
	LOOP
};

struct ArmatureCollisionRect
{
	cocos2d::Vec2 v[4];
};

// �����������
class ArmatureComponent : public anax::Component
{
public:
	ArmatureComponent() 
	{}
	// ��ǰ֡
	uint32_t curFrameIndex;
	// ��ǰ����������
	std::string curAniName;
	// �����ļ�
	std::string aniFile;
	// ��ǰ��������
	std::string curAniCMD;
	// ��ɫ��
	std::string roleName;

	kArmaturePlayMode mode;
	bool playing;
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

DEFINE_COMPONENT(ArmatureComponent);
//DEFINE_COMPONENT(ArmatureCollisionComponent);
//DEFINE_COMPONENT(CollisionFilterComponent);

