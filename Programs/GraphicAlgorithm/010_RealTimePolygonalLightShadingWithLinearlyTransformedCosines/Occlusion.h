#pragma once
#include "GameObject.h"

class COcclusion : public IGameObject
{
public:
	COcclusion(const std::string& vGameObjectName, int vExecutionOrder);

	// ͨ�� IGameObject �̳�
	virtual void initV() override;
	virtual void updateV() override;
};

