#pragma once

#include "cocos2d.h"


USING_NS_CC;


class GVirtualCamera : public cocos2d::Component
{
public:

	GVirtualCamera();

	static GVirtualCamera * create();

	void setPosition(const Vec2& pos);
	const Vec2& getPosition();

	const Vec2& getCenter();
	
	// �����ӿڴ�С
	void setViewPortSize(const Size& viewSize);

	// ���������С
	void setWorldSize(const Size& worldSize);

	// ����һ����Чλ��
	Vec2 validPosition(Vec2& pos);
	void validPositionFast(Vec2& pos);

	// ����ê��λ��
	void setCameraAnchorPoint(const Vec2& anchorPoint);


	// ���ð�Χ������İ�Χ��
	/*
								(max.x, max.y)
	-----------BoundingBox-------
	|							|
	|	 ---camera---			|
	|	 |			|			|
	|	 ------------			|
	|							|
	|---------------------------|
	(min.x, min.y)
	*/
	void setCameraBoundingBox(const Vec2& min, const Vec2& max);

	// �Ƿ����ð�Χ����ײ
	void setEnableCollision(bool enable);
	bool isEnableCollision();

	Node* getAnimationNode();

	void forceUpdate();

	void setCall(const std::function<void(float, float, float)>& call)
	{
		m_call = call;
	}

public:

	virtual bool init()override;

	virtual void update(float delta)override;

	virtual void onAdd()override;

	virtual void onRemove()override;

public:

	Node* m_animationNode;

	Size m_viewSize;
	Size m_worldSize;

	Vec2 m_cache_Position;
	float m_cache_Scale;
	
	bool m_enableCollision;

	std::function<void(float, float, float)> m_call;

	Vec2 m_cameraBoundingBoxMin;
	Vec2 m_cameraBoundingBoxMax;
	Vec2 m_cameraAnchorPoint;
};

