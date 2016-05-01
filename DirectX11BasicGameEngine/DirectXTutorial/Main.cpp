/*
	Written by Minseok Hong, GAPH in Unity Community.
	Refer to write source code :
	Book - INTRODUCTION TO 3D GAME PROGRAMMING WITH DIRECTX 11 by Frank Luna.
*/

#include "D3DMain.h"
#include "cbBufferSet.h"
#include "ConstantBuffer.h"

#include <memory>
#include <Effects.h>
#include <Windows.h>
#include <wrl/client.h>

#include "ShaderHelper.h"
#include "LightHelper.h"
#include "Geometry.h"
#include "SetParameter.h"
#include "Render.h"

class InitDirect3DApp : public D3DMain
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();
	void LoadRasterState();

private:
	//-------------- Render ���� -----------------
	Render *Rnd = new Render;
	//----------------------------------------------

	//--------------Geometry ����---------------
	Geometry *Geo = new Geometry;
	//----------------------------------------------
	//���� ����
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mHillVB;
	ID3D11Buffer* mWavesVB;
	ID3D11Buffer* mRoomVB;
	//���� ����
	ID3D11Buffer* mBoxIB;
	ID3D11Buffer* mHillIB;
	ID3D11Buffer* mWavesIB;
	//------------------------------------------

	//------------ �� ���� ���� ----------------
	DirectionalLight mDirLight;
	PointLight mPointLight;
	SpotLight mSpotLight;
	Material mLandMat;
	Material mWavesMat;
	Material mBoxMat;
	Material mWallMat;
	XMFLOAT3 mEyePosW;
	//------------------------------------------

	//------------ ���̴� ���� ���� ------------
	ID3DBlob* mPSBlob;
	ID3DBlob* mVSBlob;
	
	ID3DBlob* mTreePSBlob;
	ID3DBlob* mTreeGSBlob;
	ID3DBlob* mTreeVSBlob;

	ID3D11PixelShader* mPixelShader;
	ID3D11VertexShader* mVertexShader;

	ID3D11PixelShader* mTreePixelShader;
	ID3D11GeometryShader* mTreeGeometryShader;
	ID3D11VertexShader* mTreeVertexShader;
	//------------------------------------------

	//----------- ��� ���� ���� ---------------
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mWaveWorld;
	XMFLOAT4X4 mRoomWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	//-------------------------------------------
	
	//-------------- �ؽ��� ���� ���� --------------
	unique_ptr<DirectX::EffectFactory> m_EffectFactory;
	ComPtr<ID3D11ShaderResourceView> grass;
	ComPtr<ID3D11ShaderResourceView> Water1;
	ComPtr<ID3D11ShaderResourceView> Water2;
	ComPtr<ID3D11ShaderResourceView> WoodCrate01;
	ComPtr<ID3D11ShaderResourceView> WoodCrate02;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_D3_SamplerState;
	//-----------------------------------------------

	//----------------- ��Ÿ ���� -----------------
	SetParameter *SetPtr = new SetParameter;

	float mTheta;
	float mPhi;
	float mRadius;

	ID3D11InputLayout* mInputLayout;
	ID3D11RasterizerState* mRasterState;

	POINT mLastMousePos;
	//---------------------------------------------
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// ����� ����� �� ������� �޸� ���� ��� Ȱ��ȭ.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	InitDirect3DApp theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
	: D3DMain(hInstance), mHillVB(0), mHillIB(0),mBoxVB(0),mBoxIB(0),mWavesVB(0),mWavesIB(0),mInputLayout(0),mTheta(1.5f*MathHelper::Pi)
	,mPhi(0.25f*MathHelper::Pi),mRadius(5.0f)
{
	m_WindowCaption = L"DirectX11 Tutorial Demo";
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mBoxWorld, I);
	XMStoreFloat4x4(&mGridWorld, I);
	XMStoreFloat4x4(&mWaveWorld, I);
	XMStoreFloat4x4(&mRoomWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	XMMATRIX wavesOffset = XMMatrixTranslation(0.0f, -3.0f, 0.0f);
	XMStoreFloat4x4(&mWaveWorld, wavesOffset);

	// Directional light.
	mDirLight = SetPtr->SetDirectionalLightParameter1(mDirLight);
	//Point Light
	mPointLight = SetPtr->SetPointLightParameter(mPointLight);
	//Spot Light
	mSpotLight = SetPtr->SetSpotLightParameter(mSpotLight);

	mBoxMat = SetPtr->SetBoxMaterialParameter(mBoxMat);
	mLandMat = SetPtr->SetLandMaterialParameter(mLandMat);
	mWavesMat = SetPtr->SetWaveMaterialParameter(mWavesMat);
	mWallMat = SetPtr->SetWallMaterialParameter(mWallMat);
	
}

InitDirect3DApp::~InitDirect3DApp()
{
	//--------- ���ؽ�,�ε��� ���� ���� ���� ���� ----------
	ReleaseCOM(mHillVB);
	ReleaseCOM(mHillIB);
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mWavesVB);
	ReleaseCOM(mWavesIB);
	//-----------------------------------------------------------

	//-------------- ���̴� ���� ���� ���� ------------------
	ReleaseCOM(mPixelShader);
	ReleaseCOM(mVertexShader);
	//-----------------------------------------------------------

	//-------------- HLSL ���̴� ���� ���� ---------------
	ReleaseCOM(mPSBlob);
	ReleaseCOM(mVSBlob);
	//--------------------------------------------------------

	//-------------- ��Ÿ ���� ���� ���� ------------------
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mRasterState);
	delete Rnd;
	delete Geo;
	//--------------------------------------------------------
}

bool InitDirect3DApp::Init()
{
	if (!D3DMain::Init())
		return false;

	BuildGeometryBuffers();
	BuildFX();
	BuildVertexLayout();
	LoadRasterState();

	Rnd->Initialize(GetDevice(), GetDeviceContext());

	D3DMain::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);

	return true;
}

void InitDirect3DApp::LoadRasterState()
{
	//������ȭ�� ����
	D3D11_RASTERIZER_DESC rs;

	//�޸𸮿� ������ȭ�� ����
	memset(&rs, 0, sizeof(rs));

	rs.FillMode = D3D11_FILL_SOLID;									//������ ������ ���
	rs.CullMode = D3D11_CULL_BACK;									//�ﰢ�� ���� ���� �Ǻ�
	rs.AntialiasedLineEnable = rs.DepthClipEnable = true;			
	mRasterState = NULL;
	HR(m_D3_Device->CreateRasterizerState(&rs, &mRasterState));
}

void InitDirect3DApp::OnResize()
{
	D3DMain::OnResize();

	//â�� ũ�Ⱑ ���ϸ� ��Ⱦ�� �����ϰ� ��������� �ٽ� ����Ѵ�.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void InitDirect3DApp::UpdateScene(float dt)
{
	//���� ��ǥ�� ��ī��Ʈ ��ǥ��� ��ȯ
	float x = mRadius* sinf(mPhi)*cosf(mTheta);
	float z = mRadius* sinf(mPhi)*sinf(mTheta);
	float y = mRadius* cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	//�� ��� ����
	XMVECTOR pos = XMVectorSet(x, y,z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	Geo->UpdateGeometryWave(GetDeviceContext(),m_GameTimer.TotalTime(),&mWavesVB,&mWavesIB,dt);

	//��ó�� ������
	//��ǥ���� ���� �׸��� ������.
	mPointLight.Position.x = 70.0f*cosf(0.2f*m_GameTimer.TotalTime());
	mPointLight.Position.z = 70.0f*sinf(0.2f*m_GameTimer.TotalTime());
	mPointLight.Position.y = MathHelper::Max(
			0.3f*(mPointLight.Position.z*sinf(0.1f*mPointLight.Position.x)
			+ 
			mPointLight.Position.x*cosf(0.1f*mPointLight.Position.z)),
		-3.0f) + 10.0f;

	//ī�޶� �ٶ󺸴� ����� ������ ���� �Ĵٺ�.
	//�̷��� ������� �÷��� ����Ʈ�� ��� �ִµ��� ȿ���� ���� �� ����.
	mSpotLight.Position = mEyePosW;
	XMStoreFloat3(&mSpotLight.Direction, XMVector3Normalize(target - pos));
}

void InitDirect3DApp::DrawScene()
{
	//�Է� ��ġ�� ����ϱ� ���� ����ϰ��� �ϴ� ��ġ�� ����
	m_D3_DeviceContext->IASetInputLayout(mInputLayout);
	m_D3_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_D3_DeviceContext -> ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	m_D3_DeviceContext -> ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	m_D3_DeviceContext->PSSetShader(mPixelShader, NULL, 0);
	m_D3_DeviceContext->VSSetShader(mVertexShader, NULL, 0);

	Rnd->FrameBufferSet(GetDeviceContext(), mDirLight, mPointLight, mSpotLight, &mEyePosW);

	XMMATRIX boxScale = XMMatrixScaling(10.0f, 10.0f, 10.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(40,-3,30);
	XMStoreFloat4x4(&mBoxWorld,boxScale*boxOffset);

	Rnd->DrawGeometryObject(GetDevice(), GetDeviceContext(), mBoxWorld, mView, mProj, &mEyePosW, mBoxMat, mBoxVB, mBoxIB, Geo->GetBoxIndexCount(), Rnd->WireFence, false, true, false, false);
	Rnd->DrawGeometryObject(GetDevice(), GetDeviceContext(), mGridWorld, mView, mProj, &mEyePosW, mLandMat, mHillVB, mHillIB, Geo->GetGridIndexCount(), Rnd->grass,false,false,false,false);
	Rnd->DrawGeometryObject(GetDevice(), GetDeviceContext(), mWaveWorld, mView, mProj, &mEyePosW, mWavesMat, mWavesVB, mWavesIB, 3*Geo->GetWaveTriangleCount(), Rnd->Water2,false,false,false,true);
	Rnd->DrawWall(GetDevice(), GetDeviceContext(), mRoomWorld, mView, mProj, mWallMat, mRoomVB, Rnd->Wall);

	//������ȭ�⸦ ���������ο� ����
	m_D3_DeviceContext->RSSetState(mRasterState);

	HR(m_SwapChain->Present(0, 0));
}

void InitDirect3DApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_hWindow);
}

void InitDirect3DApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void InitDirect3DApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// �� ���� 1/4�� �°� �� �ȼ��� ����
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// ���� ���Ǹ� ���� ī�޶��� �ޱ��� ��ǲ ���� ������ ������Ʈ��
		mTheta += dx;
		mPhi += dy;

		//mphi�� �ޱ��� ������
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		//�� �ȼ��� ȭ�鿡�� 0.005���ְ� ��ġ�ϵ��� ����
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x)*10;
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y)*10;

		//��ǲ�� ������ ī�޶� �������� ������Ʈ��
		mRadius += dx - dy;

		//�������� ������
		mRadius = MathHelper::Clamp(mRadius, 10.0f, 100.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void InitDirect3DApp::BuildGeometryBuffers()
{
	Geo->BuildGeometryObject(m_D3_Device,&mBoxVB, &mBoxIB);
	Geo->BuildGeometryLand(GetDevice(), &mHillVB, &mHillIB);
	Geo->BuildGeometryWave(GetDevice(), &mWavesVB, &mWavesIB);
	Geo->BuildWallObject(GetDevice(), &mRoomVB);
}

void InitDirect3DApp::BuildFX()
{
	DWORD shaderFlags = 0;

	#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif

	//cso������ �ҷ����� ���̴��� �����
	HR(ShaderHelper::LoadCompiledShader("LightPixel.cso", &mPSBlob));
	HR(m_D3_Device->CreatePixelShader(mPSBlob->GetBufferPointer(), mPSBlob->GetBufferSize(), NULL, &mPixelShader));

	HR(ShaderHelper::LoadCompiledShader("LightVertex.cso", &mVSBlob));
	HR(m_D3_Device->CreateVertexShader(mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), NULL, &mVertexShader));
}

void InitDirect3DApp::BuildVertexLayout()
{
	//���ؽ� ��ǲ ���̾ƿ� ����
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		//���� ����ü�� �� ������ � �뵵���� �˷���
		//�ø�ƽ �̸� , �ø�ƽ ���� , �ڷ� ����, ���� ���� ���� , ������ , ���� Ŭ����, ������ ����
		{ "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL",0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//��ǲ ���̾ƿ� ����
	HR(m_D3_Device->CreateInputLayout(vertexDesc, 3, mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), &mInputLayout));
}