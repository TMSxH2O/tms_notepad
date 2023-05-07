#include "Ground.h"
#include "Interface.h"
#include "Utils.h"

CGround::CGround(const std::string& vGameObjectName, int vExecutionOrder) : IGameObject(vGameObjectName, vExecutionOrder)
{
}

void CGround::initV()
{
	GLfloat vertices[] = {
		 1.0f, 1.0f, 0.0f,  0.0f,0.0f,1.0f,   1.0f,1.0f,
		 1.0f,-1.0f, 0.0f,  0.0f,0.0f,1.0f,   1.0f,0.0f,
		-1.0f, 1.0f, 0.0f,  0.0f,0.0f,1.0f,   0.0f,1.0f,
		-1.0f, 1.0f, 0.0f,  0.0f,0.0f,1.0f,   0.0f,1.0f,
		 1.0f,-1.0f, 0.0f,  0.0f,0.0f,1.0f,   1.0f,0.0f,
		-1.0f,-1.0f, 0.0f,  0.0f,0.0f,1.0f,   0.0f,0.0f,
	};
	setVAO(createVAO(vertices, sizeof(vertices), { 3,3,2 }));
	//setVAO(ElayGraphics::ResourceManager::getOrCreateScreenQuadVAO());
	rotateX(-90);
	//scale(glm::vec3(550, 550, 1));
	scale(glm::vec3(10, 10, 1));
}

void CGround::updateV()
{
}
