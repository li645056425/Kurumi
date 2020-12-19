#pragma once

#include "ecs/anaxHelper.h"

class TransformComponent : public anax::Component
{
public:
	TransformComponent()
		: scale(1.0f)
		, rotation(.0f)
	{
	}

	// ��Ⱦ����
#if G_TARGET_SERVER
	GVec2 position;
#else
	Vec2 position;
#endif
	// �߼�����
	GVec3 logicPos;
	// ����
	float32 scale;
	// ��ת
	float32 rotation;
};


DEFINE_COMPONENT(TransformComponent);

