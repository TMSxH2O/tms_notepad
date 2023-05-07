#pragma once
#include "RenderPass.h"

class OcclusionPass : public IRenderPass
{
public:
	OcclusionPass(const std::string& vPassName, int vExcutionOrder);
	~OcclusionPass();

	virtual void initV() override;
	virtual void updateV() override;
private:
	
};

