#pragma once

#include "D3Utility.h"
#include "LightHelper.h"

class SetParameter
{
	public:
		DirectionalLight SetDirectionalLightParameter1(DirectionalLight DL);
		DirectionalLight SetDirectionalLightParameter2(DirectionalLight DL);
		DirectionalLight SetDirectionalLightParameter3(DirectionalLight DL);
		PointLight SetPointLightParameter(PointLight PL);
		SpotLight SetSpotLightParameter(SpotLight SL);
		Material SetBoxMaterialParameter(Material MT);
		Material SetLandMaterialParameter(Material MT);
		Material SetWaveMaterialParameter(Material MT);
		Material SetWallMaterialParameter(Material MT);
};