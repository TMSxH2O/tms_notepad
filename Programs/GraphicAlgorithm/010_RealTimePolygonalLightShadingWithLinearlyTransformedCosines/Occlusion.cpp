#include "Occlusion.h"
#include "Interface.h"

COcclusion::COcclusion(const std::string& vGameObjectName, int vExecutionOrder)
	: IGameObject(vGameObjectName, vExecutionOrder)
{
}

void COcclusion::initV()
{
	setVAO(ElayGraphics::ResourceManager::getOrCreateSphereVAO());
}

void COcclusion::updateV()
{
	const float occlusion_radius = ElayGraphics::ResourceManager::getSharedDataByName<float>("OcclusionRadius");
	setScale(glm::vec3(occlusion_radius));

	const glm::vec3 occlusion_pos = ElayGraphics::ResourceManager::getSharedDataByName<glm::vec3>("OcclusionPos");
	setPosition(occlusion_pos);
}
