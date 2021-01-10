#pragma once

#include "ecs/anaxHelper.h"
#include "ecs/components/UniqueComponent.h"

// ��Ա�����Ϣ
struct ActorIdentityInfo
{
	// ����ߴ�
	GVec2 bodySize;
	// ������
	GVec2 originPos;
	// ��ɫ��
	std::string roleName;
};

class CommonUtils
{
public:

	static anax::Entity& getAdmin(anax::World& world);

	static bool initMapSize(anax::Entity& admin, int mapId);

#if G_TARGET_CLIENT
	static DrawNode* getDebugDraw(anax::World& world);
#endif

	// ��Ա����
	static bool spawnActor(anax::World& world, ActorIdentityInfo& info, anax::Entity* outActor);

};
