#pragma once
#include "D3Utility.h"
#include "GeometryGenerator.h"
#include "Waves.h"
#include "GameTimer.h"
#include "Vertex.h"

class Geometry
{
public:
	//오브젝트 생성
	void BuildGeometryObject(ID3D11Device* device, ID3D11Buffer** mVB, ID3D11Buffer** mIB);
	//지형 생성
	void BuildGeometryLand(ID3D11Device* device, ID3D11Buffer** mVB, ID3D11Buffer** mIB);
	//파도 생성
	void BuildGeometryWave(ID3D11Device* device, ID3D11Buffer** mVB, ID3D11Buffer** mIB);
	//파도 움직임 업데이트
	void UpdateGeometryWave(ID3D11DeviceContext* context, float TotlaTime, ID3D11Buffer** mVB, ID3D11Buffer** mIB, float dt);
	
	void BuildWallObject(ID3D11Device* device, ID3D11Buffer** mVB);

	//----관련 오브젝트 인덱스 갯수 구하기----
	UINT GetWaveTriangleCount();
	UINT GetBoxIndexCount();
	UINT GetGridIndexCount();
	//----------------------------------------
	

	//-----------언덕용 변수 구하는 함수들 ----------
	float GetHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;
	//-----------------------------------------------

private:
	//--------------각종 변수들-------------
	int BoxVertexOffset = 0;
	UINT mBoxIndexOffset = 0;
	UINT mBoxIndexCount = 0;

	int mGridVertexOffset = 0;
	UINT mGridIndexOffset = 0;
	UINT mGridIndexCount = 0;

	Waves mWaves;
	//---------------------------------------
};