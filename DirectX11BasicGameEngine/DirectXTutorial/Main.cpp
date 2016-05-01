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
	//------------ Render Variable --------------
	//-------------- Render 변수 -----------------
	Render *Rnd = new Render;
	//----------------------------------------------

	//------------Geometry Variable -----------
	//--------------Geometry 변수---------------
	Geometry *Geo = new Geometry;
	//----------------------------------------------

	//Vertex Buffer
	//정점 버퍼
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mHillVB;
	ID3D11Buffer* mWavesVB;
	ID3D11Buffer* mRoomVB;

	//Index Buffer
	//색인 버퍼
	ID3D11Buffer* mBoxIB;
	ID3D11Buffer* mHillIB;
	ID3D11Buffer* mWavesIB;
	//------------------------------------------

	//---------- Variable With Light ----------
	//------------ 빛 관련 변수 ----------------
	DirectionalLight mDirLight;
	PointLight mPointLight;
	SpotLight mSpotLight;
	Material mLandMat;
	Material mWavesMat;
	Material mBoxMat;
	Material mWallMat;
	XMFLOAT3 mEyePosW;
	//------------------------------------------

	//---------- Variable with Shader ---------
	//------------ 쉐이더 관련 변수 ------------
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

	//--------- Variable with Matrix ----------
	//----------- 행렬 관련 변수 ---------------
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mWaveWorld;
	XMFLOAT4X4 mRoomWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	//-------------------------------------------
	
	//----------- Variable with Texture ------------
	//-------------- 텍스쳐 관련 변수 --------------
	unique_ptr<DirectX::EffectFactory> m_EffectFactory;
	ComPtr<ID3D11ShaderResourceView> grass;
	ComPtr<ID3D11ShaderResourceView> Water1;
	ComPtr<ID3D11ShaderResourceView> Water2;
	ComPtr<ID3D11ShaderResourceView> WoodCrate01;
	ComPtr<ID3D11ShaderResourceView> WoodCrate02;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_D3_SamplerState;
	//-----------------------------------------------

	//-------------- Other Variable -------------
	//----------------- 기타 변수 -----------------
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
	// Vitalization of executive time memory check when debug mode. 
	// 디버그 모드일 때 실행시점 메모리 점검 기능 활성화.
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
	//-- Release vertex and index buffer variable setting --
	//--------- 버텍스,인덱스 버퍼 변수 설정 해제 ----------
	ReleaseCOM(mHillVB);
	ReleaseCOM(mHillIB);
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mWavesVB);
	ReleaseCOM(mWavesIB);
	//-----------------------------------------------------------

	//----------- Release shader variable setting ---------
	//-------------- 쉐이더 변수 설정 해제 ------------------
	ReleaseCOM(mPixelShader);
	ReleaseCOM(mVertexShader);
	//-----------------------------------------------------------

	//--------- Release HLSL shader loaded -----------
	//-------------- HLSL 쉐이더 적재 해제 ---------------
	ReleaseCOM(mPSBlob);
	ReleaseCOM(mVSBlob);
	//--------------------------------------------------------

	//---------- Release other variable setting ----------
	//-------------- 기타 변수 설정 해제 ------------------
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
	//Create rasterizer
	//래스터화기 생성
	D3D11_RASTERIZER_DESC rs;

	//Load Rasterizer to memory
	//메모리에 래스터화기 적재
	memset(&rs, 0, sizeof(rs));

	rs.FillMode = D3D11_FILL_SOLID;									//Frame rendering mode    프레임 렌더링 모드
	rs.CullMode = D3D11_CULL_BACK;									//Check triangle selection    삼각형 선별 여부 판별
	rs.AntialiasedLineEnable = rs.DepthClipEnable = true;			
	mRasterState = NULL;
	HR(m_D3_Device->CreateRasterizerState(&rs, &mRasterState));
}

void InitDirect3DApp::OnResize()
{
	D3DMain::OnResize();

	//Recalculate perspective matrix and renew the aspect radio when window size is changed
	//창의 크기가 변하면 종횡비를 갱신하고 투영행렬을 다시 계산한다.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void InitDirect3DApp::UpdateScene(float dt)
{
	//Change spherical surface coordinate to Decartesian-coordinate system.
	//구면 좌표를 데카르트 좌표계로 변환
	float x = mRadius* sinf(mPhi)*cosf(mTheta);
	float z = mRadius* sinf(mPhi)*sinf(mTheta);
	float y = mRadius* cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	//Build view matrix
	//뷰 행렬 빌드
	XMVECTOR pos = XMVectorSet(x, y,z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	Geo->UpdateGeometryWave(GetDeviceContext(),m_GameTimer.TotalTime(),&mWavesVB,&mWavesIB,dt);

	//Act like light
	//빛처럼 동작함
	
	//Move surface like drawing circle
	//지표면을 원을 그리며 움직임.
	mPointLight.Position.x = 70.0f*cosf(0.2f*m_GameTimer.TotalTime());
	mPointLight.Position.z = 70.0f*sinf(0.2f*m_GameTimer.TotalTime());
	mPointLight.Position.y = MathHelper::Max(
			0.3f*(mPointLight.Position.z*sinf(0.1f*mPointLight.Position.x)
			+ 
			mPointLight.Position.x*cosf(0.1f*mPointLight.Position.z)),
		-3.0f) + 10.0f;

	//Look at the direction that is camera lookat
	//카메라가 바라보는 방향과 동일한 곳을 쳐다봄.

	//When using this method, able to make a effect such as holding flash light
	//이러한 방법으로 플래시 라이트를 쥐고 있는듯한 효과를 만들 수 있음.
	mSpotLight.Position = mEyePosW;
	XMStoreFloat3(&mSpotLight.Direction, XMVector3Normalize(target - pos));
}

void InitDirect3DApp::DrawScene()
{
	//Bound to device that is what you want to use for input layout
	//입력 배치를 사용하기 위해 사용하고자 하는 장치에 묶음
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

	//Bound Rasterizer to pipeline
	//래스터화기를 파이프라인에 묶기
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
		// Make each pixel to fit each degree's quater of one
		// 각 도의 1/4에 맞게 각 픽셀을 만듬
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update camera angle base on input value
		// 상자 주의를 도는 카메라의 앵글을 인풋 값에 기초해 업데이트함
		mTheta += dx;
		mPhi += dy;

		//Limit mphi's angle
		//mphi의 앵글을 제한함
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		//Make each pi
		//각 픽셀이 화면에서 0.005유닛과 일치하도록 만듬
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x)*10;
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y)*10;

		//Update radius camera base on input value
		//인풋에 기초해 카메라 반지름을 업데이트함
		mRadius += dx - dy;

		//Limit radius
		//반지름을 제한함
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

	//Make shader and load ".cso" file
	//cso파일을 불러오고 쉐이더를 만든다
	HR(ShaderHelper::LoadCompiledShader("LightPixel.cso", &mPSBlob));
	HR(m_D3_Device->CreatePixelShader(mPSBlob->GetBufferPointer(), mPSBlob->GetBufferSize(), NULL, &mPixelShader));

	HR(ShaderHelper::LoadCompiledShader("LightVertex.cso", &mVSBlob));
	HR(m_D3_Device->CreateVertexShader(mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), NULL, &mVertexShader));
}

void InitDirect3DApp::BuildVertexLayout()
{
	//Create vertex input layout
	//버텍스 인풋 레이아웃 제작
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		//Inform usage information that is vertex structure's each elements
		//정점 구조체의 각 성분을 어떤 용도인지 알려줌
		// Sementic name, Sementic index, Data form, Buffer slot index, Offset, Slot class, Data interval
		//시맨틱 이름 , 시맨틱 색인 , 자료 형식, 버퍼 슬롯 색인 , 오프셋 , 슬롯 클래스, 데이터 간격
		{ "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL",0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//Make input layout
	//인풋 레이아웃 제작
	HR(m_D3_Device->CreateInputLayout(vertexDesc, 3, mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), &mInputLayout));
}