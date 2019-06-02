#pragma once

#include "cocos2d.h"

using namespace cocos2d;

#define BOX2D_PIXEL_TO_METER 30.0f

enum BOX2D_FILTER_MASK
{
	B2DM_GROUND		= 1 << 1, // ����
	B2DM_SIDE_WALL	= 1 << 2, // ǽ��
	B2DM_PLAYER		= 1 << 3, // ���
	B2DM_MONSTER	= 1 << 4, // ����
	B2DM_SKILL		= 1 << 5, // ����
};

#define GAMEWORLD_NODE_MAP 0
#define GAMEWORLD_NODE_UI  1
#define GAMEWORLD_NODE_MAX 2


#if COCOS2D_DEBUG == 1 
#define ENABLE_ARMATURE_COLLISION_DRAW_DEBUG 1
#define ENABLE_BOX2D_DEBUG_DRAW
#else
#define ENABLE_ARMATURE_COLLISION_DRAW_DEBUG 0
#endif