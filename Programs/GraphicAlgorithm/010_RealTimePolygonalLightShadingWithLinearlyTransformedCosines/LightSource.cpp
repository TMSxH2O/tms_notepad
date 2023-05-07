#include "LightSource.h"
#include "Interface.h"

CLightSource::CLightSource(const std::string &vGameObjectName, int vExecutionOrder) : IGameObject(vGameObjectName, vExecutionOrder)
{
}

//************************************************************************************
//Function:
void CLightSource::initV()
{
	setVAO(ElayGraphics::ResourceManager::getOrCreateScreenQuadVAO());
	translate(glm::vec3(0, 1, 0));
	m_PolygonalLightVertexOriginalPosSet =
	{
		glm::vec3(1, 1, 0),
		glm::vec3(1, -1, 0),
		glm::vec3(-1, -1, 0),
		glm::vec3(-1, 1, 0)
	};
	m_PolygonalLightVertexPosSet.resize(m_PolygonalLightVertexOriginalPosSet.size());
	for (int i = 0; i < m_PolygonalLightVertexPosSet.size(); ++i)
	{
		m_PolygonalLightVertexPosSet[i] = glm::vec3(getModelMatrix() * glm::vec4(m_PolygonalLightVertexOriginalPosSet[i], 1.0));
	}
}

//************************************************************************************
//Function:
void CLightSource::updateV()
{
	bool has_update = false;
	const glm::vec3 min_flt_vec3(0.00001);
	glm::vec3 rotation = ElayGraphics::ResourceManager::getSharedDataByName<glm::vec3>("Rotation");
	if (glm::any(glm::greaterThan(glm::abs(rotation - getRotation()), min_flt_vec3))) {
		setRotation(rotation);
		has_update = true;
	}

	glm::vec3 position = ElayGraphics::ResourceManager::getSharedDataByName<glm::vec3>("Position");
	if (glm::any(glm::greaterThan(glm::abs(position - getPosition()), min_flt_vec3))) {
		setPosition(position);
		has_update = true;
	}

	if (has_update) {
		for (int i = 0; i < m_PolygonalLightVertexPosSet.size(); ++i)
		{
			m_PolygonalLightVertexPosSet[i] = glm::vec3(getModelMatrix() * glm::vec4(m_PolygonalLightVertexOriginalPosSet[i], 1.0));
		}
	}
}