#pragma once

#include "ecs/anaxHelper.h"
#include "ecs/utils/physics/GShape.h"

enum GBodyType
{
	STATIC_BODY,
	DYNAMIC_BODY
};

class BodyComponent : public anax::Component
{
public:
	BodyComponent()
	{
		shape = NULL;
		angularVelocity = 0.0f;
		torque = 0.0f;
		radians = 0.0f;
		staticFriction = 0.5f;
		dynamicFriction = 0.3f;
		restitution = 0.2f;
		bodyType = GBodyType::DYNAMIC_BODY;
	}

	virtual ~BodyComponent()
	{
		if (shape)
			delete shape;
	}

	GBodyType bodyType;

	// λ��
	GVec2 position;
	// �ٶ�
	GVec2 velocity;

	// ���ٶ�
	real angularVelocity;
	// Ť��
	real torque;
	// ��ת(������)
	real radians;
	// ��
	GVec2 force;

	// ת������,����ٶȼ������
	real I;
	//  1 / ת������
	real iI;
	// ����
	real m;
	//  1 / ����
	real im;

	// http://gamedev.tutsplus.com/tutorials/implementation/how-to-create-a-custom-2d-physics-engine-friction-scene-and-jump-table/
	// ��̬Ħ����
	real staticFriction;
	// ��̬Ħ����
	real dynamicFriction;
	// ����
	real restitution;

	GShape* shape;
};
