#pragma once
#ifndef cbBufferSet_h__
#define cbBufferSet_h__
#include <DirectXMath.h>
#include "LightHelper.h"

using namespace DirectX;

struct cbPerFrame
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	XMFLOAT3 gEyePosW;

	float gFogStart;
	float gFogRange;
	XMFLOAT4 gFogColor;
};

struct cbPerObject
{
	XMFLOAT4X4 gWorld;
	XMFLOAT4X4 gWorldInvTranspose;
	XMFLOAT4X4 gWorldViewProj;
	XMFLOAT4X4 gTexScale;
	Material gMaterial;
};

struct cbTreePerObject
{
	XMFLOAT4 gViewProj;
	Material gMaterial;
};

struct cbTreePerFrame
{
	DirectionalLight gDirLights[3];
	XMFLOAT3 gEyePosW;

	float  gFogStart;
	float  gFogRange;
	XMFLOAT4 gFogColor;
};



#endif // cbBufferSet_h__