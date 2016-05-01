#include "Render.h"

void Render::DrawGeometryObject(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4X4 mObWorld, XMFLOAT4X4 mVie, XMFLOAT4X4 mPro, XMFLOAT3* mEyePosW, Material mMat, ID3D11Buffer* mVB, ID3D11Buffer* mIB, UINT IndexCount,
	ComPtr<ID3D11ShaderResourceView> Texture, bool Wfrs, bool Ncrs, bool Atcbs, bool Tbs)
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//Create vertex buffer
	//�������� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//Setting matrix vertex
	//��� ���� ����
	XMMATRIX world = XMLoadFloat4x4(&mObWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX view = XMLoadFloat4x4(&mVie);
	XMMATRIX proj = XMLoadFloat4x4(&mPro);
	XMMATRIX TexScale;
	XMMATRIX viewproj = view*proj;

	XMMATRIX worldViewProj = XMMatrixTranspose(world*view*proj);

	//------ Bound matrix that is will included in constant Buffer ------
	//----------- Constant Buffer�� ���Ե� ������ ��� ���� -------------
	cbPerObject mPerObjectCB;

	XMStoreFloat4x4(&mPerObjectCB.gWorld, XMMatrixTranspose(world));
	XMStoreFloat4x4(&mPerObjectCB.gWorldInvTranspose, worldInvTranspose);
	XMStoreFloat4x4(&mPerObjectCB.gWorldViewProj, worldViewProj);
	mPerObjectCB.gMaterial = mMat;
	
	mObjectConstantBuffer.Data = mPerObjectCB;
	mObjectConstantBuffer.ApplyChanges(context);
	//---------------------------------------------------------------------------

	//Bound vertex, index buffer to pipeline
	//���������ο� ��������, ���ι��� ����
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
		//						(ȥ�ջ��°�ü, RGBA���� ����, ����ǥ��ȭ ǥ��)
		context->OMSetBlendState(TransparentBS, blendFactor, 0xffffffff);
	
	context->PSSetShaderResources(0, 1, Texture.GetAddressOf());

	//Draw basic figure using index buffer
	//���� ���۸� �̿��� �⺻���� �׸���
	//In calling draw function, this is value that is added index count, init index location, and other index values
	//�׸��� ȣ�⿡�� ����� ���ε��� ����, ù ������ ��ġ, ���ε鿡 �������� ������
	context->DrawIndexed(IndexCount, 0, 0);
	
	if (Ncrs || Wfrs)
		context->RSSetState(0);
	if (Tbs || Atcbs)
		context->OMSetBlendState(0, blendFactor, 0xffffffff);
}

void Render::DrawWall(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4X4 mObWorld, XMFLOAT4X4 mVie, XMFLOAT4X4 mPro, Material mMat, ID3D11Buffer* mVB, ComPtr<ID3D11ShaderResourceView> Texture)
{
	//Cretae vertex buffer
	//�������� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Setting Constant Value
	//��� ����
	XMMATRIX world = XMLoadFloat4x4(&mObWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX view = XMLoadFloat4x4(&mVie);
	XMMATRIX proj = XMLoadFloat4x4(&mPro);
	XMMATRIX TexScale;
	XMMATRIX viewproj = view*proj;

	XMMATRIX worldViewProj = XMMatrixTranspose(world*view*proj);

	cbPerObject mPerObjectCB;

	//--------------- Create Object ------------------
	//--------------------��ü ����--------------------
	XMStoreFloat4x4(&mPerObjectCB.gWorld, XMMatrixTranspose(world));
	XMStoreFloat4x4(&mPerObjectCB.gWorldInvTranspose, worldInvTranspose);
	XMStoreFloat4x4(&mPerObjectCB.gWorldViewProj, worldViewProj);
	mPerObjectCB.gMaterial = mMat;

	mObjectConstantBuffer.Data = mPerObjectCB;
	mObjectConstantBuffer.ApplyChanges(context);

	//Bound vertex buffer to pipeline
	//���������ο� �������� ����
	context->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

	auto buffer = mObjectConstantBuffer.Buffer();
	context->PSSetConstantBuffers(1, 1, &buffer);
	context->VSSetConstantBuffers(1, 1, &buffer);


	context->PSSetShaderResources(0, 1, Texture.GetAddressOf());

	//Draw basic figure using index buffer
	//���� ���۸� �̿��� �⺻���� �׸���
	//In calling draw function, this is value that is added index count, init index location, and other index values
	//�׸��� ȣ�⿡�� ����� ���ε��� ����, ù ������ ��ġ, ���ε鿡 �������� ������
	context->Draw(18, 6);
}

void Render::DrawWallStencil(ID3D11Device* device, ID3D11DeviceContext* context, XMFLOAT4X4 mObWorld, XMFLOAT4X4 mVie, XMFLOAT4X4 mPro, ID3D11Buffer* mVB)
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	//Create vertex buffer
	//�������� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;


	//Setting constant
	//��� ����
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
	//---------------�ؽ��� ����----------------------
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
	//----------------------ȥ��-------------------------
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
	alphaCoverageDesc.AlphaToCoverageEnable = true;					//Setting acception or rejection that is alpha inclusion convert of vitalization   ���� ������ ��ȯ Ȱ��ȭ ���� ����
	alphaCoverageDesc.IndependentBlendEnable = false;				//Setting acception or rejection that is each rendering target's conversion fixing   ���� ��󸶴� ȥ���� �������� �����ϴ� �Ϳ� ���� ���� ����
	alphaCoverageDesc.RenderTarget[0].BlendEnable = false;			//ist elements is included mix setting structures that submit to ist render target  i��° ���Ҵ� ���� ���� ����� i��° ���� ��� ������ ȥ�� ������ ���� ����ü
	alphaCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&alphaCoverageDesc, &AlphaToCoverageBS));

	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;										//Setting mix vitalization or not vitalization  ȥ�� Ȱ��ȭ, ��Ȱ��ȭ ���� ����
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;						//RGB element's original mix calculation  RGB���� ȥ���� ���� ȥ�� ����� ����
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;					//RGB element's original mix calculation  RGB���� ȥ���� ��� ȥ�� ����� ����
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;							//RGB element's original mix calculation  RGB���� ȥ���� ��� ȥ�� ����� ����
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;						//Alpha element's original mix calculation ���� ���� ȥ���� ���� ȥ�� ����� ����
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;						//Alpha element's original mix calculation  ���� ���� ȥ���� ��� ȥ�� ����� ����
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;						//Alpha element's original mix operator  ���� ���� ȥ���� ȥ�� �����ڸ� ����
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//Mask of render target's write ���� ��� ���� ����ũ
	HR(device->CreateBlendState(&transparentDesc, &TransparentBS));
	//-----------------------------------------------------

	//------------ Setting Sampler ---------------
	//----------------���÷� ����------------------
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
	//���÷� ����
	context->PSSetSamplers(0, 1, m_D3_SamplerState.GetAddressOf());
	//--------------------------------------------------

	mObjectConstantBuffer.Initialize(device);
	mFrameConstantBuffer.Initialize(device);
}
