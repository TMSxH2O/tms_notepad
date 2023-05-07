#include "LightCamera.h"
#include "Interface.h"
#include <glm/gtc/matrix_transform.hpp>

CLightCamera::CLightCamera(const std::string& vGameObjectName, int vExecutionOrder) : IGameObject(vGameObjectName, vExecutionOrder)
{
}

CLightCamera::~CLightCamera()
{
}

void CLightCamera::initV()
{
	m_LightPos = ElayGraphics::ResourceManager::getSharedDataByName<glm::vec3>("LightPos");
	m_LightDir = ElayGraphics::ResourceManager::getSharedDataByName<glm::vec3>("LightDir");
	ElayGraphics::Camera::setMainCameraPos({ -0.163989, -1.29911, 2.43392 });
	ElayGraphics::Camera::setMainCameraFront({ 0.0103159, -0.171929, -0.985055 });
	ElayGraphics::Camera::setMainCameraMoveSpeed(0.1);
	if (abs(m_LightDir) == m_LightUpVector)
		m_LightDir.z += 0.01f;
	m_LightViewMatrix = glm::lookAt(m_LightPos, m_LightPos + m_LightDir, m_LightUpVector);
	m_LightProjectionMatrix = glm::ortho(-m_CameraSizeExtent, m_CameraSizeExtent, -m_CameraSizeExtent, m_CameraSizeExtent, 0.1f, 7.5f);
	ElayGraphics::ResourceManager::registerSharedData("LightViewMatrix", m_LightViewMatrix);
	ElayGraphics::ResourceManager::registerSharedData("LightProjectionMatrix", m_LightProjectionMatrix);
	ElayGraphics::ResourceManager::registerSharedData("LightCameraAreaInWorldSpace", (2.0f * m_CameraSizeExtent) * (2.0f * m_CameraSizeExtent));
	ElayGraphics::ResourceManager::registerSharedData("m_CameraSizeExtent", m_CameraSizeExtent);
	ElayGraphics::ResourceManager::registerSharedData("Exposure", m_Exposure);
}

void CLightCamera::updateV()
{
}