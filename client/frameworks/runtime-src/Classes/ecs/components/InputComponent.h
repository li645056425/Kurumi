#pragma once

#include "ecs/anaxHelper.h"
#include "ecs/utils/opmsg/GOPMsg.h"

class InputComponent : public BaseComponent
{
public:

	InputComponent()
	{
		lastKeyDown = G_KEY_NONE;
		keyDown = G_KEY_NONE;
	}

	// ��һ֡����״̬
	G_BIT_TYPE lastKeyDown;
	// ���µļ�
	G_BIT_TYPE keyDown;
};

