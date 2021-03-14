#pragma once

#include "ecs/anaxHelper.h"
#include "opmsg/GOPMsgQue.h"

#if G_TARGET_CLIENT
#include "foundation/render/GMapLayer.h"
#endif

class GlobalComponent : public BaseComponent
{
public:
	GlobalComponent()
	{
		fAccumilatedTime = 0.0f;
		fNextGameTime = 0.0f;
		gameLogicFrame = 0U;

#if G_TARGET_CLIENT
		mapRender = NULL;
		debugDrawNode = NULL;
#endif
		mapWidth = 0.0f;
		mapHeight = 0.0f;
		minPosy = 0.0f;
		maxPosy = 0.0f;
	}

#if G_TARGET_CLIENT
	GMapLayer*	mapRender;
	DrawNode* debugDrawNode;
#endif


	///! ֡ͬ�����
	// �ۼ�����ʱ��
	float32 fAccumilatedTime;
	// ��һ���߼�֡ʱ��
	float32 fNextGameTime;
	// ��Ϸ�߼�֡��
	uint32_t gameLogicFrame;


	///! ��ͼ���
	// ��ͼ��/��
	float32 mapWidth;
	float32 mapHeight;
	// ��ͼY����С/���ֵ
	float32 minPosy;
	float32 maxPosy;


	// �����Ϣ����
	GOPMsgQue inputQue;
	// �����Ϣ����
	GOPMsgQue outputQue;
};
