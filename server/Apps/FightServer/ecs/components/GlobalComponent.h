#pragma once

#include "ecs/anaxHelper.h"


class GlobalComponent : public BaseComponent
{
public:
	GlobalComponent()
	{
		fAccumilatedTime = 0.0f;
		fNextGameTime = 0.0f;
		gameLogicFrame = 0U;
	}

#if G_TARGET_SERVER

#else
	cocos2d::Node* stageNode;
	cocos2d::Node* mapNode;
#endif


	// �ۼ�����ʱ��
	float32 fAccumilatedTime;
	// ��һ���߼�֡ʱ��
	float32 fNextGameTime;
	// ��Ϸ�߼�֡��
	uint32_t gameLogicFrame;
};
