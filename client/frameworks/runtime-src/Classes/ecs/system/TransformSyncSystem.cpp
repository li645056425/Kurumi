#include "TransformSyncSystem.h"

void TransformSyncSystem::sync()
{
	const auto& entities = this->getEntities();
	for (auto& it : entities)
	{
		auto& transform = it.getComponent<TransformComponent>();
		auto& simphys = it.getComponent<SIMPhysComponent>();
		transform.logicPos.x = simphys.position.x;
		transform.logicPos.y = simphys.position.y;
		// �߼�����ת��Ļ����
		//transform.position
	}
}
//
//void TransformSyncSystem::syncRender()
//{
//#if G_TARGET_SERVER
//#else
//	const auto& entities = this->getEntities();
//	for (auto& it : entities)
//	{
//		auto& component = it.getComponent<ArmatureRenderComponent>();
//		if (component.render)
//		{
//			component.render->setPosition(it.getComponent<TransformComponent>().position);
//		}
//	}
//#endif
//}
