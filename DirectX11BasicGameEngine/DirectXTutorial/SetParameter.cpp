#include "SetParameter.h"

DirectionalLight SetParameter::SetDirectionalLightParameter1(DirectionalLight DL)
{
	DL.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	DL.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	DL.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	DL.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	return DL;
}
DirectionalLight SetParameter::SetDirectionalLightParameter2(DirectionalLight DL)
{
	DL.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	DL.Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	DL.Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	DL.Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	return DL;
}
DirectionalLight SetParameter::SetDirectionalLightParameter3(DirectionalLight DL)
{
	DL.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	DL.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	DL.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	DL.Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	return DL;
}

PointLight SetParameter::SetPointLightParameter(PointLight PL)
{
	PL.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	PL.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	PL.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	PL.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	PL.Range = 25.0f;
	return PL;
}

SpotLight SetParameter::SetSpotLightParameter(SpotLight SL)
{
	SL.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	SL.Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	SL.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SL.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	SL.Spot = 96.0f;
	SL.Range = 10000.0f;
	return SL;
}

Material SetParameter::SetBoxMaterialParameter(Material MT)
{
	MT.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	MT.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MT.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	return MT;
}

Material SetParameter::SetLandMaterialParameter(Material MT)
{
	MT.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	MT.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MT.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	return MT;
}

Material SetParameter::SetWaveMaterialParameter(Material MT)
{
	MT.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	MT.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	MT.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
	return MT;
}

Material SetParameter::SetWallMaterialParameter(Material MT)
{
	MT.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	MT.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MT.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	return MT;
}