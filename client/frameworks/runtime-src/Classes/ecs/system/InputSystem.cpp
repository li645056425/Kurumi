#include "InputSystem.h"
#include "ecs/system/SIMPhysSystem.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/utils/CommonUtils.h"


void InputSystem::update()
{
	auto& entities = this->getEntities();
	// ������һ֡����״̬
	for (auto& entity : entities)
	{
		auto& inputComponent = entity.getComponent<InputComponent>();
		inputComponent.lastKeyDown = inputComponent.keyDown;
		inputComponent.keyDown = G_KEY_NONE;
	}

	auto& globalCom = CommonUtils::getGlobalComponent(getWorld());
	while (globalCom.inputQue.check(globalCom.gameLogicFrame))
	{
		auto msg = globalCom.inputQue.popMsg();

		for (auto& entity : entities)
		{
			//if (msg->uuid == entity.getComponent<PropertyComponent>().uuid)
			{
				auto& inputComponent = entity.getComponent<InputComponent>();
				inputComponent.keyDown = G_BIT_SET(inputComponent.keyDown, msg->keydown);
			}
		}
		globalCom.inputQue.freeMsg(msg);
	}

	// ����״̬����
	for (auto& entity : entities)
	{
		auto& inputComponent = entity.getComponent<InputComponent>();
		if (inputComponent.keyDown != G_KEY_NONE || inputComponent.lastKeyDown != G_KEY_NONE)
		{
			// ���ȴ���̧���¼�
			for (auto i = 1; i <= G_KEY_MAX_COUNT; ++i)
			{
				if (G_BIT_EQUAL(inputComponent.lastKeyDown, G_FIXED_VALUE << i))
				{
					if (G_BIT_EQUAL(inputComponent.keyDown, G_FIXED_VALUE << i))
					{
						// ������������
						keepPress(entity, G_FIXED_VALUE << i);
					}
					else
					{
						// ����̧��
						keyUp(entity, G_FIXED_VALUE << i);
					}
				}
			}

			for (auto i = 1; i <= G_KEY_MAX_COUNT; ++i)
			{
				if (G_BIT_NO_EQUAL(inputComponent.lastKeyDown, G_FIXED_VALUE << i))
				{
					// ��������˲��
					if (G_BIT_EQUAL(inputComponent.keyDown, G_FIXED_VALUE << i))
					{
						keyDown(entity, G_FIXED_VALUE << i);
					}
				}
			}
		}
	}
}

void InputSystem::keyUp(const anax::Entity& entity, G_BIT_TYPE key)
{
	auto& property = entity.getComponent<PropertyComponent>();
	property.stateMachine->onKeyUp(key);
	G_LOG_I("keyUp %d", key);
}

void InputSystem::keyDown(const anax::Entity& entity, G_BIT_TYPE key)
{
	auto& property = entity.getComponent<PropertyComponent>();
	property.stateMachine->onKeyDown(key);
	G_LOG_I("keyDown %d", key);
}

void InputSystem::keepPress(const anax::Entity& entity, G_BIT_TYPE key)
{
	auto& property = entity.getComponent<PropertyComponent>();
	property.stateMachine->onKeepPress(key);
	G_LOG_I("keepPress %d", key);
}


