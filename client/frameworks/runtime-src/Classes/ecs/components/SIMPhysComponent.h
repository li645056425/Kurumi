#pragma once

#include "ecs/anaxHelper.h"
#include "foundation/math/GMath.h"

class SIMPhysComponent : public BaseComponent
{
public:
	SIMPhysComponent()
	{
		//linearDamping = 0.2f;
		gravityScale = 1.0f;
		isStatic = false;
	}

	bool isStatic;


	// ������,ֱ�ӽ�������Ϊ1
	//float32 mass
	//float32 invMass

	// ������,��������
	// ��������
	//float32 linearDamping;

	// ��û�п��������ĵط�����������Ӱ���½��ٶȡ�
	// Ҫģ����ʵ���������ʹ������ϵ��,����ʹ������ģ�����Ħ�����µļ������
	float32 gravityScale;


	// �����ٶ�
	GVec2 linearVelocity;

	// ��
	GVec2 force;

	// postion + size = rect
	GVec2 position;
	GVec2 size;
};
