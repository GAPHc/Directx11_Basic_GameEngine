#pragma once
#include "ConstantBuffer.h"
#include "D3Utility.h"
#include "cbBufferSet.h"
#include "Vertex.h"
#include "LightHelper.h"
#include "Texture.h"

#include <Effects.h>
#include <wrl/client.h>

using namespace std;
using namespace Microsoft::WRL;

class Render
{
public:
	//Draw Object     ������Ʈ �׸���
	void DrawGeometryObject(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4X4 mObWorld, XMFLOAT4X4 mVie, XMFLOAT4X4 mPro, XMFLOAT3* mEyePosW, Material mMat , ID3D11Buffer* mVB, ID3D11Buffer* mIB, UINT IndexCount, 
		ComPtr<ID3D11ShaderResourceView> Texture, bool Wfrs, bool Ncrs, bool Atcbs, bool Tbs);

	//Initalize frame buffer    ������ ���� �ʱ�ȭ
	void FrameBufferSet(ID3D11DeviceContext* context, DirectionalLight DL, PointLight PL, SpotLight SL, XMFLOAT3* EyePos);

	//Initalize sampler,texture,constant buffer  ���÷�,�ؽ���,��� ���� �ʱ�ȭ
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);

	void DrawWall(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4X4 mObWorld, XMFLOAT4X4 mVie, XMFLOAT4X4 mPro, Material mMat, ID3D11Buffer* mVB, ComPtr<ID3D11ShaderResourceView> Texture);
	void DrawWallStencil(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4X4 mObWorld, XMFLOAT4X4 mVie, XMFLOAT4X4 mPro, ID3D11Buffer* mVB);

	//---------------- Texture ---------------
	//------------------�ؽ���------------------
	ComPtr<ID3D11ShaderResourceView> grass;
	ComPtr<ID3D11ShaderResourceView> Water1;
	ComPtr<ID3D11ShaderResourceView> Water2;
	ComPtr<ID3D11ShaderResourceView> WireFence;
	ComPtr<ID3D11ShaderResourceView> WoodCrate01;
	ComPtr<ID3D11ShaderResourceView> WoodCrate02;
	ComPtr<ID3D11ShaderResourceView> Wall;
	//-------------------------------------------

private:
	//------ Constant buffer variable -------
	//------------- ������� ����--------------
	ConstantBuffer<cbPerObject> mObjectConstantBuffer;
	ConstantBuffer<cbPerFrame> mFrameConstantBuffer;
	//-----------------------------------------

	ID3D11RasterizerState* WireframeRS;
	ID3D11RasterizerState* NoCullRS;
	ID3D11BlendState* AlphaToCoverageBS;
	ID3D11BlendState* TransparentBS;

	//Effect factory
	//����Ʈ ���丮
	unique_ptr<DirectX::EffectFactory> m_EffectFactory;

	//Texture sampler
	//�ؽ��� ���÷�
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_D3_SamplerState;
};