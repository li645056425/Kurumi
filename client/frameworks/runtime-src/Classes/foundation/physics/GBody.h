#pragma once

#include "foundation/math/GMath.h"

class GShape;

enum GBodyType
{
	STATIC_BODY,
	DYNAMIC_BODY
};

class GBody
{
public:

	GBody(GShape *shape_, uint32_t x, uint32_t y);


	// ʩ����
	void applyForce(const GVec2& f)
	{
		force += f;
	}

	// ʩ�ӳ���
	void applyImpulse(const GVec2& impulse, const GVec2& contactVector)
	{
		velocity += im * impulse;
		angularVelocity += iI * cross(contactVector, impulse);
	}

	// ��������Ϊ��̬����
	void setStatic(void)
	{
		type = GBodyType::STATIC_BODY;
		I = 0.0f;
		iI = 0.0f;
		m = 0.0f;
		im = 0.0f;
	}

	// ��ת����
	void setOrient(real radians);

	// λ��
	GVec2 position;
	// �ٶ�
	GVec2 velocity;

	// ���ٶ�
	real angularVelocity;
	// Ť��
	real torque; 
	// ��ת(������)
	real orient;
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

	// Shape interface
	GShape *shape;

	GBodyType type;
};
