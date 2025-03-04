﻿#pragma once

#include "GGameWorld.h"

#include "mugen/component/GActorComponent.h"
#include "mugen/component/GStaticDataComponent.h"
#include "mugen/component/GRigidBodyComponent.h"
#include "mugen/component/GAvatarComponent.h"
#include "mugen/component/GCollisionComponent.h"
#include "mugen/component/GDataComponent.h"

NS_G_BEGIN

GGameWorld* GGameWorld::sGameWorldInstance = NULL;

GGameWorld* GGameWorld::getInstance()
{
	return sGameWorldInstance;
}

GGameWorld* GGameWorld::create(int32_t mapId, uint32_t randomSeed, Node* rootNode)
{
	GGameWorld* ptr = new GGameWorld();
	if (ptr && ptr->init(mapId, randomSeed, rootNode))
	{
		return ptr;
	}
	G_SAFE_DELETE(ptr);
	return NULL;
}

GGameWorld::GGameWorld()
	: m_pRootNode(NULL)
	, m_pMapLayer(NULL)
	, m_pRandom(NULL)
	, m_localPlayer(NULL)
	, m_enableDebugDraw(false)
{
	m_gameLogicFrame = 0;
	m_fAccumilatedTime = 0.0f;

	m_pEntityManager = GEntityManager::getInstance();
	m_pEntityManager->clear();

	G_ASSERT(sGameWorldInstance == NULL);
	sGameWorldInstance = this;
	m_pScheduler = new Scheduler();

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("rect_collide.plist");
	//Director::getInstance()->setAnimationInterval(LogicInterval.to_float());
}

GGameWorld::~GGameWorld()
{
	G_ASSERT(sGameWorldInstance == this);
	sGameWorldInstance = NULL;

	CC_SAFE_RELEASE_NULL(m_pScheduler);

	G_SAFE_DELETE(m_pRandom);

	GEntityManager::destroy();
}

bool GGameWorld::init(int32_t mapId, uint32_t randomSeed, Node* rootNode)
{
	m_pRootNode = rootNode;

	if (!initMapInfo(mapId))
	{
		return false;
	}

	G_SAFE_DELETE(m_pRandom);
	m_pRandom = new GRandom(randomSeed, randomSeed + 1);

	return true;
}

bool GGameWorld::initMapInfo(int32_t mapId)
{
	m_pMapLayer = GMapLayer::create(mapId);
	if (m_pMapLayer == NULL)
		return false;

	m_pRootNode->addChild(m_pMapLayer);

	// 初始化世界边界碰撞体

	fixedPoint mapWidth = m_pMapLayer->getMapWidth();
	fixedPoint mapHeight = m_pMapLayer->getMapHeight();

	m_physSystem.setMapBox(GFixedVec2(0.0f, m_pMapLayer->getMinPosY() / PHYSICS_PIXEL_TO_METER), GFixedVec2(mapWidth / PHYSICS_PIXEL_TO_METER, m_pMapLayer->getMaxPosY() / PHYSICS_PIXEL_TO_METER));


	//fixedPoint space = 0.0f;
	//fixedPoint h_height = mapHeight - space * 2.0f;
	//fixedPoint v_width = mapWidth - space * 2.0f;
	//
	////  left
	//{
	//	auto pComponent = new GRigidBodyComponent();
	//	pComponent->setStaticBox(GFixedVec2(space, 0.0f), GFixedVec2(10.0f, h_height), GFixedVec2(1.0f, 0.0f));
	//	m_pEntityManager->newEntity()->addComponent(pComponent);
	//}
	//// right
	//{
	//	auto pComponent = new GRigidBodyComponent();
	//	pComponent->setStaticBox(GFixedVec2(mapWidth - space, 0.0f), GFixedVec2(10.0f, h_height), GFixedVec2(0.0f, 0.0f));
	//	m_pEntityManager->newEntity()->addComponent(pComponent);
	//}

	//// top
	//{
	//	
	//	auto pComponent = new GRigidBodyComponent();
	//	pComponent->setStaticBox(GFixedVec2(space, m_pMapLayer->getMaxPosY()), GFixedVec2(v_width, 10.0f), GFixedVec2(0.0f, 0.0f));
	//	m_pEntityManager->newEntity()->addComponent(pComponent);
	//}
	//// bottom
	//{
	//	auto pComponent = new GRigidBodyComponent();
	//	pComponent->setStaticBox(GFixedVec2(space, m_pMapLayer->getMinPosY()), GFixedVec2(v_width, 10.0f), GFixedVec2(0.0f, 1.0f));
	//	m_pEntityManager->newEntity()->addComponent(pComponent);
	//}


	return true;
}

void GGameWorld::step()
{
	// 输入更新
	m_inputSystem.step();

	// 物理更新
	m_physSystem.step();

	// 碰撞更新
	m_collisionSystem.step();

	// 逻辑更新 update
	m_pEntityManager->update();


	m_fAccumilatedTime += LogicInterval;
	// 必须放在最后，不然Input系统更新的逻辑帧数对不上输入消息的帧数
	m_gameLogicFrame++;


	if (m_localPlayer)
	{
		auto& pos = G_GetComponent(m_localPlayer, GActorComponent)->transform.p;
		auto camera = getMapLayer()->getVirtualCamera();
		camera->setPosition(Vec2(pos.x.to_float(), pos.y.to_float()));
	}
}

void GGameWorld::render(float dt)
{
#if G_DEBUG
	m_pMapLayer->getDrawNode()->clear();

	if (m_enableDebugDraw)
		m_physSystem.debugDraw();
#endif

	m_pScheduler->update(dt);
	m_pEntityManager->renderUpdate(dt);

	m_pMapLayer->getVirtualCamera()->doUpdate(dt);
}

void GGameWorld::input(uint32_t entityId, uint32_t frame, G_BIT_TYPE keydown)
{
	m_inputSystem.getOPMsgQue()->addMsg<GOPMsg>(frame, entityId, keydown);
}

GEntity* GGameWorld::spwanActor(const std::string& filename, const GFixedVec3& pos, const std::string& replaceArmatureFile)
{
	auto data = cocos2d::FileUtils::getInstance()->getDataFromFile(filename);
	if (data.getSize() <= 0)
		return NULL;

	GByteBuffer buffer(data.getBytes(), data.getSize());

	auto pActorComponent = new GActorComponent();
	auto pStaticDataComponent = new GStaticDataComponent();
	auto pRigidBodyComponent = new GRigidBodyComponent();
	auto pAvatarComponent = new GAvatarComponent();
	auto pCollisionComponent = new GCollisionComponent();
	auto pDataComponent = new GDataComponent();

	bool ok = false;
	do
	{
		G_BREAK_IF(!pActorComponent->deserialize(buffer));
		G_BREAK_IF(!pStaticDataComponent->deserialize(buffer));

		if (replaceArmatureFile.empty() == false)
		{
			pStaticDataComponent->armatureFile = replaceArmatureFile;
		}
		
		pRigidBodyComponent->setMass(pStaticDataComponent->mass);
		pRigidBodyComponent->setBox(pos, pStaticDataComponent->size, GFixedVec3(0.5f, 0.0f, 0.5f));

		ok = true;
	} while (false);

	if (false == ok)
	{
		delete pActorComponent;
		delete pStaticDataComponent;
		delete pRigidBodyComponent;
		delete pAvatarComponent;
		delete pDataComponent;
		return NULL;
	}

	auto entity = m_pEntityManager->newEntity();
	entity->addComponent(pActorComponent);
	entity->addComponent(pStaticDataComponent);
	entity->addComponent(pRigidBodyComponent);
	entity->addComponent(pAvatarComponent);
	entity->addComponent(pCollisionComponent);
	entity->addComponent(pDataComponent);

	return entity;
}

void GGameWorld::enableDebugDraw(bool enable)
{
#if G_DEBUG
	m_enableDebugDraw = enable;
	m_collisionSystem.setEnableDebugDraw(enable);
#endif
}

void GGameWorld::setScheduler(Node* node, bool recursive/* = true*/)
{
	node->setScheduler(m_pScheduler);

	if (recursive)
	{
		auto& children = node->getChildren();
		for (auto i = 0; i < children.size(); ++i)
		{
			this->setScheduler(children.at(i), recursive);
		}
	}
}

NS_G_END
