#pragma once

#include "ecs/anaxHelper.h"

class PropertyComponent : BaseComponent
{
public:

	PropertyComponent()
	{
		isJumping = false;
		faceRight = true;
	}

	bool isJumping;
	GVec2 moveForce;

	// �泯�Ҳ�
	bool faceRight;
};

