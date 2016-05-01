#pragma once
#include "D3Utility.h"
#include "GeometryGenerator.h"
#include "Waves.h"
#include "GameTimer.h"
#include "Vertex.h"

class Geometry
{
public:
	//������Ʈ ����
	void BuildGeometryObject(ID3D11Device* device, ID3D11Buffer** mVB, ID3D11Buffer** mIB);
	//���� ����
	void BuildGeometryLand(ID3D11Device* device, ID3D11Buffer** mVB, ID3D11Buffer** mIB);
	//�ĵ� ����
	void BuildGeometryWave(ID3D11Device* device, ID3D11Buffer** mVB, ID3D11Buffer** mIB);
	//�ĵ� ������ ������Ʈ
	void UpdateGeometryWave(ID3D11DeviceContext* context, float TotlaTime, ID3D11Buffer** mVB, ID3D11Buffer** mIB, float dt);
	
	void BuildWallObject(ID3D11Device* device, ID3D11Buffer** mVB);

	//----���� ������Ʈ �ε��� ���� ���ϱ�----
	UINT GetWaveTriangleCount();
	UINT GetBoxIndexCount();
	UINT GetGridIndexCount();
	//----------------------------------------
	

	//-----------����� ���� ���ϴ� �Լ��� ----------
	float GetHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;
	//-----------------------------------------------

private:
	//--------------���� ������-------------
	int BoxVertexOffset = 0;
	UINT mBoxIndexOffset = 0;
	UINT mBoxIndexCount = 0;

	int mGridVertexOffset = 0;
	UINT mGridIndexOffset = 0;
	UINT mGridIndexCount = 0;

	Waves mWaves;
	//---------------------------------------
};