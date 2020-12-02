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
	GVec2 position;
	// �߼�����
	GVec3 logicPos;
	// ����
	float32 scale;
	// ��ת
	float32 rotation;
};


DEFINE_COMPONENT(TransformComponent);

