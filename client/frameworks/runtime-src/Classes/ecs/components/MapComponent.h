#pragma once

#include "ecs/anaxHelper.h"

#if G_TARGET_CLIENT
#include "foundation/render/GMapLayer.h"
#endif

// �����������
class MapComponent : public anax::Component
{
public:
	MapComponent()
	{
#if G_TARGET_CLIENT
		render = NULL;
#endif
	}

#if G_TARGET_CLIENT
	GMapLayer* render;
#endif
};
