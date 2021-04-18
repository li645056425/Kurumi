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

	afterInput();
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

void InputSystem::input(GOPMsg_Base* msg)
{
	auto& entities = this->getEntities();
	for (auto& entity : entities)
	{
		if (msg->uuid == entity.getComponent<PropertyComponent>().uuid)
		{
			auto& inputComponent = entity.getComponent<InputComponent>();
			// ����̧��
			if (G_BIT_EQUAL(msg->cmd, G_CMD_KEY_UP))
			{
#if 0
				auto tempValue = inputComponent.keyDown;
#endif
				auto msgKey = (GOPMsg_Key*)msg;
				G_BIT_REMOVE(inputComponent.keyDown, msgKey->key);

#if 0
				// ��֤һ������ķ����ǶԵĲ�
				for (auto i = 1; i < G_BIT_MAX_COUNT; ++i)
				{
					if (G_BIT_EQUAL(msgKey->key, G_FIXED_VALUE << i))
						G_BIT_REMOVE(tempValue, G_FIXED_VALUE << i);
				}
				G_ASSERT(tempValue == inputComponent.keyDown);
#endif
			}
			// ��������
			else if (G_BIT_EQUAL(msg->cmd, G_CMD_KEY_DOWN))
			{
				auto msgKey = (GOPMsg_Key*)msg;
				G_BIT_SET(inputComponent.keyDown, msgKey->key);
			}
		}
	}
}

void InputSystem::afterInput()
{
	auto keys = getAutoResetKeys();
	auto& entities = this->getEntities();
	for (auto& entity : entities)
	{
		auto& input = entity.getComponent<InputComponent>();
		auto& property = entity.getComponent<PropertyComponent>();
		property.stateMachine->updateInput();

		// �Զ���ԭ����
		G_BIT_REMOVE(input.keyDown, keys);
	}
}