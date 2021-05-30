#include "InputSystem.h"
#include "ecs/system/SIMPhysSystem.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/utils/CommonUtils.h"


void InputSystem::update()
{
	beforeInput();

	auto& globalCom = CommonUtils::getGlobalComponent(getWorld());

	while (globalCom.inputQue.check(globalCom.gameLogicFrame))
	{
		auto msg = globalCom.inputQue.popMsg();
		input(msg);
		globalCom.inputQue.freeMsg(msg);
	}

	auto& entities = this->getEntities();
	for (auto& entity : entities)
	{
		auto& property = entity.getComponent<PropertyComponent>();
		property.stateMachine->updateInput();
	}
}

void InputSystem::beforeInput()
{
	auto& entities = this->getEntities();

	// ������һ֡����״̬
	for (auto& entity : entities)
	{
		auto& inputComponent = entity.getComponent<InputComponent>();
		inputComponent.lastKeyDown = inputComponent.keyDown;
	}

	// ����Ƿ�λ�ڿ���
	for (auto& entity : entities)
	{
		auto& propertyCom = entity.getComponent<PropertyComponent>();
		auto& simphysCom = entity.getComponent<SIMPhysComponent>();
		if (float_equal(simphysCom.linearVelocity.y, 0.0f))
		{
			G_BIT_REMOVE(propertyCom.status, G_PS_IS_IN_AIR);
			propertyCom.jumpCount = 0;
		}
		else
		{
			G_BIT_SET(propertyCom.status, G_PS_IS_IN_AIR);
		}
	}
}

void InputSystem::input(GOPMsg* msg)
{
	auto& entities = this->getEntities();
	for (auto& entity : entities)
	{
		if (msg->uuid == entity.getComponent<PropertyComponent>().uuid)
		{
			auto& inputComponent = entity.getComponent<InputComponent>();

			for (auto i = 1; i <= G_KEY_MAX_COUNT; ++i)
			{
				if (G_BIT_EQUAL(inputComponent.lastKeyDown, G_FIXED_VALUE << i))
				{
					// ����̧��
					if (G_BIT_NO_EQUAL(inputComponent.keyDown, G_FIXED_VALUE << i))
					{
						keyUp(entity, G_FIXED_VALUE << i);
					}
					// ������������
					else
					{
					}
				}
				else
				{
					// ��������˲��
					if (G_BIT_EQUAL(inputComponent.keyDown, G_FIXED_VALUE << i))
					{
					}
				}
			}
		}
	}
}

void InputSystem::keyUp(const anax::Entity& entity, G_BIT_TYPE key)
{}



