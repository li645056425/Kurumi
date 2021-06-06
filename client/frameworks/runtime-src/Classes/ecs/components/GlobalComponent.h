#pragma once

#include "ecs/anaxHelper.h"
#include "ecs/utils/opmsg/GOPMsgQue.h"
#include "foundation/GRandom.h"
#include "foundation/render/GMapLayer.h"

class GlobalComponent : public BaseComponent
{
public:
	GlobalComponent()
	{
		fAccumilatedTime = 0.0f;
		gameLogicFrame = 0U;
		uuidSeed = 0;

		mapRender = NULL;
		debugDrawNode = NULL;
		mapWidth = 0.0f;
		mapHeight = 0.0f;
		minPosy = 0.0f;
		maxPosy = 0.0f;
	}

	GMapLayer*	mapRender;
	DrawNode* debugDrawNode;


	///! ֡ͬ�����
	// �ۼ�����ʱ��
	float32 fAccumilatedTime;
	// ��Ϸ�߼�֡��
	uint32_t gameLogicFrame;


	///! ��ͼ���
	// ��ͼ��/��
	float32 mapWidth;
	float32 mapHeight;
	// ��ͼY����С/���ֵ
	float32 minPosy;
	float32 maxPosy;


	// ������Ϣ����
	GOPMsgQue inputQue;

	// �����������
	std::unique_ptr<GRandom> random;

	// GUUID����
	GUUID uuidSeed;
};
