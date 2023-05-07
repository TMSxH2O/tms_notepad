#include <GLM/gtc/type_ptr.hpp>

#include "MyGUI.h"
#include "Interface.h"
#include "Common.h"

CMyGUI::CMyGUI(const std::string& vName, int vExcutionOrder) : IGUI(vName, vExcutionOrder)
{
}

CMyGUI::~CMyGUI()
{
}

//************************************************************************************
//Function:
void CMyGUI::initV()
{
	ElayGraphics::ResourceManager::registerSharedData("DiffuseColor", m_DiffuseColor);
	ElayGraphics::ResourceManager::registerSharedData("SpecularColor", m_SpecularColor);
	ElayGraphics::ResourceManager::registerSharedData("Roughness", m_Roughness);
	ElayGraphics::ResourceManager::registerSharedData("Intensity", m_Intensity);
	ElayGraphics::ResourceManager::registerSharedData("IsTwoSide", m_IsTwoSide);
	ElayGraphics::ResourceManager::registerSharedData("Rotation", m_LightSourceRotation);
	ElayGraphics::ResourceManager::registerSharedData("Position", m_LightSourcePosition);

	ElayGraphics::ResourceManager::registerSharedData("OcclusionPos", m_OcclusionPosition);
	ElayGraphics::ResourceManager::registerSharedData("OcclusionRadius", m_OcclusionRadius);
}

//************************************************************************************
//Function:
void CMyGUI::updateV()
{
	if (IGUI::colorEdit("Diffuse Color", m_DiffuseColor))
		ElayGraphics::ResourceManager::updateSharedDataByName("DiffuseColor", m_DiffuseColor);
	if (IGUI::colorEdit("Specular Color", m_SpecularColor))
		ElayGraphics::ResourceManager::updateSharedDataByName("SpecularColor", m_SpecularColor);
	float MinRoughness = 0.0f, MaxRoughness = 1.0f;
	if (IGUI::sliderScalar("Roughness", ElayGraphics::EDataType::DataType_Float, &m_Roughness, &MinRoughness, &MaxRoughness))
		ElayGraphics::ResourceManager::updateSharedDataByName("Roughness", m_Roughness);
	float MinIntensity = 0.0f, MaxIntensity = 10.0f;
	if (IGUI::sliderScalar("Intensity", ElayGraphics::EDataType::DataType_Float, &m_Intensity, &MinIntensity, &MaxIntensity))
		ElayGraphics::ResourceManager::updateSharedDataByName("Intensity", m_Intensity);
	float MinPosition = -5.f, MaxPosition = 5.f;
	if (IGUI::sliderScalarN("Position", ElayGraphics::EDataType::DataType_Float, 3, glm::value_ptr(m_LightSourcePosition), &MinPosition, &MaxPosition))
		ElayGraphics::ResourceManager::updateSharedDataByName("Position", m_LightSourcePosition);
	float MinRotation = 0.0f, MaxRotation = 360.0f;
	if (IGUI::sliderScalarN("Rotation", ElayGraphics::EDataType::DataType_Float, 3, glm::value_ptr(m_LightSourceRotation), &MinRotation, &MaxRotation))
		ElayGraphics::ResourceManager::updateSharedDataByName("Rotation", m_LightSourceRotation);

	if (IGUI::checkBox("IsTwoSide", m_IsTwoSide))
		ElayGraphics::ResourceManager::updateSharedDataByName("IsTwoSide", m_IsTwoSide);

	if (IGUI::sliderScalarN("OcclusionPos", ElayGraphics::EDataType::DataType_Float, 3, glm::value_ptr(m_OcclusionPosition), &MinPosition, &MaxPosition)) {
		ElayGraphics::ResourceManager::updateSharedDataByName("OcclusionPos", m_OcclusionPosition);
	}
	float MinRadius = 0.1, MaxRadius = 0.5;
	if (IGUI::sliderScalar("OcclusionRadius", ElayGraphics::EDataType::DataType_Float, &m_OcclusionRadius, &MinRadius, &MaxRadius)) {
		ElayGraphics::ResourceManager::updateSharedDataByName("OcclusionRadius", m_OcclusionRadius);
	}
}