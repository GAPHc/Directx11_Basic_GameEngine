#include "Render.h"

void Render::DrawGeometryObject(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4X4 mObWorld, XMFLOAT4X4 mVie, XMFLOAT4X4 mPro, XMFLOAT3* mEyePosW, Material mMat, ID3D11Buffer* mVB, ID3D11Buffer* mIB, UINT IndexCount,
	ComPtr<ID3D11ShaderResourceView> Texture, bool Wfrs, bool Ncrs, bool Atcbs, bool Tbs)
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//Create vertex buffer
	//정점버퍼 생성
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//Setting matrix vertex
	//행렬 변수 설정
	XMMATRIX world = XMLoadFloat4x4(&mObWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX view = XMLoadFloat4x4(&mVie);
	XMMATRIX proj = XMLoadFloat4x4(&mPro);
	XMMATRIX TexScale;
	XMMATRIX viewproj = view*proj;

	XMMATRIX worldViewProj = XMMatrixTranspose(world*view*proj);

	//------ Bound matrix that is will included in constant Buffer ------
	//----------- Constant Buffer에 포함될 변수의 행렬 묶기 -------------
	cbPerObject mPerObjectCB;

	XMStoreFloat4x4(&mPerObjectCB.gWorld, XMMatrixTranspose(world));
	XMStoreFloat4x4(&mPerObjectCB.gWorldInvTranspose, worldInvTranspose);
	XMStoreFloat4x4(&mPerObjectCB.gWorldViewProj, worldViewProj);
	mPerObjectCB.gMaterial = mMat;
	
	mObjectConstantBuffer.Data = mPerObjectCB;
	mObjectConstantBuffer.ApplyChanges(context);
	//---------------------------------------------------------------------------

	//Bound vertex, index buffer to pipeline
	//파이프라인에 정점버퍼, 색인버퍼 묶기
	context->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	context->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
 
	auto buffer = mObjectConstantBuffer.Buffer();
	context->PSSetConstantBuffers(1, 1, &buffer);
	context->VSSetConstantBuffers(1, 1, &buffer);
	
	if (Wfrs)
		context->RSSetState(WireframeRS);
	if (Ncrs)
		context->RSSetState(NoCullRS);
	if (Atcbs)
		context->OMSetBlendState(AlphaToCoverageBS, blendFactor, 0xffffffff);
	if (Tbs)
		//						(MixStateObject, RGBA color vector, Multisampling sample) 
		//						(혼합상태객체, RGBA색상 백터, 다중표본화 표본)
		context->OMSetBlendState(TransparentBS, blendFactor, 0xffffffff);
	
	context->PSSetShaderResources(0, 1, Texture.GetAddressOf());

	//Draw basic figure using index buffer
	//색인 버퍼를 이용해 기본도형 그리기
	//In calling draw function, this is value that is added index count, init index location, and other index values
	//그리기 호출에서 사용할 색인들의 개수, 첫 색인의 위치, 색인들에 더해지는 정수값
	context->DrawIndexed(IndexCount, 0, 0);
	
	if (Ncrs || Wfrs)
		context->RSSetState(0);
	if (Tbs || Atcbs)
		context->OMSetBlendState(0, blendFactor, 0xffffffff);
}

void Render::DrawWall(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4X4 mObWorld, XMFLOAT4X4 mVie, XMFLOAT4X4 mPro, Material mMat, ID3D11Buffer* mVB, ComPtr<ID3D11ShaderResourceView> Texture)
{
	//Cretae vertex buffer
	//정점버퍼 생성
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Setting Constant Value
	//상수 설정
	XMMATRIX world = XMLoadFloat4x4(&mObWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX view = XMLoadFloat4x4(&mVie);
	XMMATRIX proj = XMLoadFloat4x4(&mPro);
	XMMATRIX TexScale;
	XMMATRIX viewproj = view*proj;

	XMMATRIX worldViewProj = XMMatrixTranspose(world*view*proj);

	cbPerObject mPerObjectCB;

	//--------------- Create Object ------------------
	//--------------------물체 생성--------------------
	XMStoreFloat4x4(&mPerObjectCB.gWorld, XMMatrixTranspose(world));
	XMStoreFloat4x4(&mPerObjectCB.gWorldInvTranspose, worldInvTranspose);
	XMStoreFloat4x4(&mPerObjectCB.gWorldViewProj, worldViewProj);
	mPerObjectCB.gMaterial = mMat;

	mObjectConstantBuffer.Data = mPerObjectCB;
	mObjectConstantBuffer.ApplyChanges(context);

	//Bound vertex buffer to pipeline
	//파이프라인에 정점버퍼 묶기
	context->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

	auto buffer = mObjectConstantBuffer.Buffer();
	context->PSSetConstantBuffers(1, 1, &buffer);
	context->VSSetConstantBuffers(1, 1, &buffer);


	context->PSSetShaderResources(0, 1, Texture.GetAddressOf());

	//Draw basic figure using index buffer
	//색인 버퍼를 이용해 기본도형 그리기
	//In calling draw function, this is value that is added index count, init index location, and other index values
	//그리기 호출에서 사용할 색인들의 개수, 첫 색인의 위치, 색인들에 더해지는 정수값
	context->Draw(18, 6);
}

void Render::DrawWallStencil(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4X4 mObWorld, XMFLOAT4X4 mVie, XMFLOAT4X4 mPro, ID3D11Buffer* mVB)
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	//Create vertex buffer
	//정점버퍼 생성
	UINT stride = sizeof(Vertex);
	UINT offset = 0;


	//Setting constant
	//상수 설정
	XMMATRIX world = XMLoadFloat4x4(&mObWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX view = XMLoadFloat4x4(&mVie);
	XMMATRIX proj = XMLoadFloat4x4(&mPro);
	XMMATRIX TexScale;
	XMMATRIX viewproj = view*proj;

	XMMATRIX worldViewProj = XMMatrixTranspose(world*view*proj);


}

void Render::FrameBufferSet(ID3D11DeviceContext* context, DirectionalLight DL, PointLight PL, SpotLight SL, XMFLOAT3* EyePos)
{
	cbPerFrame mPerFrameCB;

	mPerFrameCB.gDirLight = DL;
	mPerFrameCB.gPointLight = PL;
	mPerFrameCB.gSpotLight = SL;
	mPerFrameCB.gEyePosW = *EyePos;
	mPerFrameCB.gFogStart = 15.0f;
	mPerFrameCB.gFogRange = 175.0f;
	mPerFrameCB.gFogColor = XMFLOAT4(0.835294f, 0.835294f, 0.835294f, 1.0f);

	mFrameConstantBuffer.Data = mPerFrameCB;
	mFrameConstantBuffer.ApplyChanges(context);

	auto buffer = mFrameConstantBuffer.Buffer();
	context->PSSetConstantBuffers(0, 1, &buffer);
}

void Render::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	//----------- Setting Texture --------------------
	//---------------텍스쳐 세팅----------------------
	m_EffectFactory = std::unique_ptr<EffectFactory>(new EffectFactory(device));
	m_EffectFactory->SetDirectory(L"..\\Asset\\");

	m_EffectFactory->CreateTexture(L"Texture\\grass.dds", context, &grass);
	m_EffectFactory->CreateTexture(L"Texture\\water1.dds", context, &Water1);
	m_EffectFactory->CreateTexture(L"Texture\\water2.dds", context, &Water2);
	m_EffectFactory->CreateTexture(L"Texture\\WoodCrate01.dds", context, &WoodCrate01);
	m_EffectFactory->CreateTexture(L"Texture\\WoodCrate02.dds", context, &WoodCrate02);
	m_EffectFactory->CreateTexture(L"Texture\\WireFence.dds", context, &WireFence);
	m_EffectFactory->CreateTexture(L"Texture\\brick01.dds", context, &Wall);
	//----------------------------------------------------

	//------------------- Mixing -----------------------
	//----------------------혼합-------------------------
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&wireframeDesc, &WireframeRS));

	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&noCullDesc, &NoCullRS));

	D3D11_BLEND_DESC alphaCoverageDesc = { 0 };
	alphaCoverageDesc.AlphaToCoverageEnable = true;					//Setting acception or rejection that is alpha inclusion convert of vitalization   알파 포괄도 변환 활성화 여부 설정
	alphaCoverageDesc.IndependentBlendEnable = false;				//Setting acception or rejection that is each rendering target's conversion fixing   렌더 대상마다 혼합을 개별정을 수정하는 것에 대한 여부 설정
	alphaCoverageDesc.RenderTarget[0].BlendEnable = false;			//ist elements is included mix setting structures that submit to ist render target  i번째 원소는 다중 렌더 대상의 i번째 렌더 대상에 적용할 혼합 설정을 담은 구조체
	alphaCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&alphaCoverageDesc, &AlphaToCoverageBS));

	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;										//Setting mix vitalization or not vitalization  혼합 활성화, 비활성화 여부 설정
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;						//RGB element's original mix calculation  RGB성분 혼합의 원본 혼합 계수를 뜻함
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;					//RGB element's original mix calculation  RGB성분 혼합의 대상 혼합 계수를 뜻함
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;							//RGB element's original mix calculation  RGB성분 혼합의 대상 혼합 계수를 뜻함
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;						//Alpha element's original mix calculation 알파 성분 혼합의 원본 혼합 계수를 뜻함
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;						//Alpha element's original mix calculation  알파 성분 혼합의 대상 혼합 계수를 뜻함
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;						//Alpha element's original mix operator  알파 성분 혼합의 혼합 연산자를 뜻함
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//Mask of render target's write 렌더 대상 쓰기 마스크
	HR(device->CreateBlendState(&transparentDesc, &TransparentBS));
	//-----------------------------------------------------

	//------------ Setting Sampler ---------------
	//----------------샘플러 설정------------------
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	HR(device->CreateSamplerState(&samplerDesc, &m_D3_SamplerState));
	//Insert Sampler
	//샘플러 삽입
	context->PSSetSamplers(0, 1, m_D3_SamplerState.GetAddressOf());
	//--------------------------------------------------

	mObjectConstantBuffer.Initialize(device);
	mFrameConstantBuffer.Initialize(device);
}
