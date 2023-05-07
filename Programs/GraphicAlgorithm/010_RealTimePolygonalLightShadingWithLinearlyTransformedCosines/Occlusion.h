#pragma once
#include "GameObject.h"

class COcclusion : public IGameObject
{
public:
	COcclusion(const std::string& vGameObjectName, int vExecutionOrder);

	// Í¨¹ý IGameObject ¼Ì³Ð
	virtual void initV() override;
	virtual void updateV() override;
};

