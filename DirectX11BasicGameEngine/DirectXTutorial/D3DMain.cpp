#include <windowsx.h>
#include "D3DMain.h"
#include<sstream>

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	D3DMain* gd3dApp = 0;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

// Constructor
// ������
D3DMain::D3DMain(HINSTANCE hInstance)
	//Initalize window variables
	//Window ���� ���� �ʱ�ȭ
	:m_hInstance(hInstance),
	m_hWindow(0),
	m_WindowCaption(L"D3D11 Application"),
	m_WindowWidth(600),
	m_WindowHeight(400),
	m_isPaused(false),
	m_isMinimized(false),
	m_isMaximized(false),
	m_isResizing(false),

	//Initalize window variables
	//DirectX ���� ���� �ʱ�ȭ
	m_D3_Device(0),
	m_D3_DeviceContext(0),
	m_SwapChain(0),
	m_DepthStencilBuffer(0),
	m_DepthStencilView(0),
	m_RenderTargetView(0),
	m_4XMSAA_quality(0),
	m_Enable4XMSAA(false),

	//Initalize other variables
	//��Ÿ ���� �ʱ�ȭ
	m_CurrentFramePerSeconds(0.0f)
{
	ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));
	gd3dApp = this;
}
//Desturctor
//�Ҹ���
D3DMain::~D3DMain()
{
	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_SwapChain);
	ReleaseCOM(m_DepthStencilBuffer);

	if (m_D3_DeviceContext)
		m_D3_DeviceContext->ClearState();

	ReleaseCOM(m_D3_DeviceContext);
	ReleaseCOM(m_D3_Device);
}
//-- Framework method --
// -- �����ӿ�ũ �޼ҵ� --
//Initalize
//�ʱ�ȭ
bool D3DMain::Init()
{
	if (!MakeWindow())
		return false;
	if (!InitDirectX3D())
		return false;

	return true;
}

//Resize Of window
//â ũ�� ����
void D3DMain::OnResize()
{
	//ReDraw
	//�ٽ� ��ο�
	ReDraw();
}
//Update Scene
//ȭ�� ������Ʈ
void D3DMain::UpdateScene(float deltaTime)
{

}
//Draw Scene
//ȭ�� �׸���
void D3DMain::DrawScene()
{
}
//Message processor
//�޽��� ó����
LRESULT D3DMain::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE) { //Activate window â Ȱ��ȭ
			m_isPaused = true;
			m_GameTimer.Stop();
		}
		else { //Inactivate window â ��Ȱ��ȭ
			m_isPaused = false;
			m_GameTimer.Start();
		}
		return 0;

	case WM_SIZE: //Change window size  â ũ�� ����
		return 0;
	case WM_ENTERSIZEMOVE: //Start window size control â ũ�� ���� ����
		m_isPaused = true;
		m_isResizing = true;
		m_GameTimer.Stop();
		return 0;
	case WM_EXITSIZEMOVE: //End of window size control â ũ�� ���� ��
		m_isPaused = false;
		m_isResizing = false;
		m_GameTimer.Start();
		OnResize();
		return 0;

	case WM_DESTROY: //Destroy window   â ����
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_GETMINMAXINFO: //Set window's max/min  â �ִ�/�ּ� ����
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

		//Mouse event  ���콺 �̺�Ʈ
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//-- Other Method --
// -- ��Ÿ �޼ҵ� --

//Run
//����
int D3DMain::Run()
{
	MSG msg = { 0 };
	//Reset init timer
	//�ʱ� Ÿ�̸� ����
	m_GameTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		//If message exist, message execute 
		//�޽����� ������ ����
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else//if not, time execute�ƴҰ�� �ð� ����
		{
			m_GameTimer.Tick();

			if (!m_isPaused)
			{
				CalculateFrameStats();
				UpdateScene(m_GameTimer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}
//Change instance handle
//�ν��Ͻ� �ڵ� ��ȯ
HINSTANCE D3DMain::GetInstanceHandle()const
{
	return m_hInstance;
}
//Change window handle
//������ �ڵ� ��ȯ
HWND D3DMain::GetWindowHandle()const
{
	return m_hWindow;
}
//Change aspect of radio
//��Ⱦ�� ȭ�� ���� ��ȯ
float D3DMain::GetAspectRatio()const
{
	return static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
}
//Change FPS
//FPS��ȯ
float D3DMain::GetCurrentFramePerSeconds()const
{
	return m_CurrentFramePerSeconds;
}

//Create Window
//������ ����
bool D3DMain::MakeWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return false;
	}

	//Window rect size calculation based on client area size
	//Ŭ���̾�Ʈ ������ ũ�⿡ ����� ������ �簢�� ũ�⸦ ���
	RECT R = { 0, 0, m_WindowWidth,m_WindowHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hWindow = CreateWindow(L"D3DWndClassName", m_WindowCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hInstance, 0);
	if (!m_hWindow)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hWindow, SW_SHOW);
	UpdateWindow(m_hWindow);

	return true;
}
//Directx init
//���̷�ƮX �ʱ�ȭ
bool D3DMain::InitDirectX3D()
{
	//Extrenal graphics 
	//����׷��� 
	IDXGIFactory* factory;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&factory));
	IDXGIAdapter* adapter;
	factory->EnumAdapters(1, &adapter);

	//Create directx device
	//DirectX ��ġ ����
	UINT flag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL d3_featureLevel;
	HRESULT res = D3D11CreateDevice(
		adapter, //Display adaptor   ���÷��� ���
		D3D_DRIVER_TYPE_UNKNOWN, //Driver type   ����̹� Ÿ��
		0, //Softwar driver  ����Ʈ���� ������
		flag, //Flag   �÷���
		0, //Function level array    ��� ���� �迭(�������)
		0, //Functon level array size   ��� ���� �迭 ũ��
		D3D11_SDK_VERSION, //Version of sdk    SDK ����
		&m_D3_Device, //Change devie  ��ġ ��ȯ
		&d3_featureLevel, //Retrun Featurelevel �����Ǵ� ��� ���� ��ȯ
		&m_D3_DeviceContext //Return DeviceContext ��ġ ���� ��ȯ
		);

	if (FAILED(res)) //Create of device fail ��ġ ���� ����
	{
		MessageBox(0, L"Fail to create directx11 device.", 0, 0);
		return false;
	}

	if (d3_featureLevel != D3D_FEATURE_LEVEL_11_0) //Not support of Directx 11 DirectX 11 ���� ����
	{
		MessageBox(0, L"this machine is not support directx11.", 0, 0);
		return false;
	}

	//Check 4X MSAA support
	//4X MSAA ���� ���� ����
	HR(m_D3_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4XMSAA_quality));
	assert(m_4XMSAA_quality > 0);

	//Set change chain
	//��ȯ �罽 ����
	DXGI_SWAP_CHAIN_DESC swapChainDescribe;
	//Set back side buffer
	//�ĸ� ���� ����
	swapChainDescribe.BufferDesc.Width = m_WindowWidth; //Back side buffer width �ĸ� ���� �ʺ�
	swapChainDescribe.BufferDesc.Height = m_WindowHeight; //Back side buffer height �ĸ� ���� ����
	swapChainDescribe.BufferDesc.RefreshRate.Numerator = 60; //Replacement rate(child) ������(����)
	swapChainDescribe.BufferDesc.RefreshRate.Denominator = 1; //Replacement rate(parent) ������(�и�)
	swapChainDescribe.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //Set resource type ���ҽ� ���� ����
	swapChainDescribe.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //Set scanline mode ��ĵ���� ��� ����
	swapChainDescribe.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //Set scale mode ��� ��� ����

	//Set multisampling 
	//����ǥ��ȭ����
	//If support about 4X MSAA, set 4X
	//4X MSAA�� ����� ��� 4��� ����
	if (m_Enable4XMSAA)
	{
		swapChainDescribe.SampleDesc.Count = 4;
		swapChainDescribe.SampleDesc.Quality = m_4XMSAA_quality - 1;
	}
	else
	{
		swapChainDescribe.SampleDesc.Count = 1;
		swapChainDescribe.SampleDesc.Quality = 0;
	}

	//Other Setting
	//��Ÿ ����
	swapChainDescribe.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //Usage of Buffer(For rendering) ���� ��� �뵵(��������)
	swapChainDescribe.BufferCount = 1; //Count of back buffer �ĸ� ���� ����
	swapChainDescribe.OutputWindow = m_hWindow; //Window of display result ����� ǥ���� ������
	swapChainDescribe.Windowed = true; //Set window mode â��� ����
	swapChainDescribe.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //Set change effect ��ȯ ȿ�� ����
	swapChainDescribe.Flags = 0; //Other flag ��Ÿ �÷���

	//Create change chain
	//��ȯ �罽 ����
	IDXGIDevice* dxgiDevice = 0;
	HR(m_D3_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(m_D3_Device, &swapChainDescribe, &m_SwapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	return true;

}

//-- Running --
//--- ������ ---

//Redraw 
//�ٽ� �׸�
bool D3DMain::ReDraw()
{
	assert(m_D3_DeviceContext);
	assert(m_D3_Device);
	assert(m_SwapChain);

	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_DepthStencilBuffer);
	// Resetting render target view, change chain
	// ũ�⿡ �°� ��ȯ �罽 / Render Target View �缳��
	HR(m_SwapChain->ResizeBuffers(1, m_WindowWidth, m_WindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_D3_Device->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView));
	ReleaseCOM(backBuffer);

	// Set depth/stencil view (texture)
	// ����/���ٽ� �� ����(�ؽ���)
	D3D11_TEXTURE2D_DESC depthStencilDescribe;
	depthStencilDescribe.Width = m_WindowWidth; //Texture width �ؽ��� �ʺ�
	depthStencilDescribe.Height = m_WindowHeight; //Texture height �ؽ��� ����
	depthStencilDescribe.MipLevels = 1; //Mip Level �Ӹ� ����
	depthStencilDescribe.ArraySize = 1; //Textuer Count �ؽ��� ����
	depthStencilDescribe.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //Type of texture �ؽ��� ����

	//Set multisampling 
	//����ǥ��ȭ����
	//If support about 4X MSAA, set 4X
	//4X MSAA�� ����� ��� 4��� ����
	if (m_Enable4XMSAA)
	{
		depthStencilDescribe.SampleDesc.Count = 4;
		depthStencilDescribe.SampleDesc.Quality = m_4XMSAA_quality - 1;
	}
	else
	{
		depthStencilDescribe.SampleDesc.Count = 1;
		depthStencilDescribe.SampleDesc.Quality = 0;
	}
	depthStencilDescribe.Usage = D3D11_USAGE_DEFAULT;  //Usage ( DEFAULT )   ��� �뵵 ( DEFAULT )
	depthStencilDescribe.BindFlags = D3D11_BIND_DEPTH_STENCIL; //Method of bound to pipeline ���������ο� ���̴� ���
	depthStencilDescribe.CPUAccessFlags = 0; //Set CPU approach   CPU ���� ����
	depthStencilDescribe.MiscFlags = 0; //Other flag ��Ÿ �÷���

	// Creat depth_stencil view
	// ����_���ٽ� �� ����
	HR(m_D3_Device->CreateTexture2D(
		&depthStencilDescribe, //Set   ����
		0, //Init data   �ʱ� �ڷ�
		&m_DepthStencilBuffer //Return buffer   ���� ��ȯ
		));
	HR(m_D3_Device->CreateDepthStencilView(
		m_DepthStencilBuffer, //Depth Buffer  ���� ����
		0, //set   ����
		&m_DepthStencilView //Return view  �� ��ȯ
		));

	//Bound to output collator state    ��� ���ձ� �ܰ迡 ����
	m_D3_DeviceContext->OMSetRenderTargets(
		1, //Render count    ���� ����
		&m_RenderTargetView, //Render view array pointer    ���� �� �迭 ������ 
		m_DepthStencilView //Depth_stencil veiw    ����_���ٽ� ��
		);

	
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.Width = static_cast<float>(m_WindowWidth);
	viewPort.Height = static_cast<float>(m_WindowHeight);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	m_D3_DeviceContext->RSSetViewports(
		1,  //Viewport Count   ����Ʈ ����
		&viewPort //Veiwport array pointer  ����Ʈ �迭 ������ 
		);

	return true;
}
//Calculate frame state
//������ ���� ���
void D3DMain::CalculateFrameStats()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;
	// Calculate framerate after 1 second 
	// 1�ʰ� ���� ������ �� ���
	if (m_GameTimer.TotalTime() - timeElapsed >= 1.0f)
	{
		float fps = (float)frameCount;
		float mspf = 1000.0f / fps;

//#if defined(DEBUF) } defined(_DEBUF)
		std::wostringstream outs;
		outs.precision(6);
		outs << m_WindowCaption << L"    " << L"FPS : " << fps << L"     " << L" Frame Time : " << mspf << L"( ms)" << L"4XMASS : " << m_4XMSAA_quality;
		SetWindowText(m_hWindow, outs.str().c_str());
//#endif
		m_CurrentFramePerSeconds = fps;
		
		//Init
		//�ʱ�ȭ
		frameCount = 0;
		timeElapsed += 1.0f;
	}
}

GameTimer D3DMain::GetGameTimer()
{
	return m_GameTimer;
}

ID3D11Device* D3DMain::GetDevice()
{
	return m_D3_Device;
}


ID3D11DeviceContext* D3DMain::GetDeviceContext()
{
	return m_D3_DeviceContext;
}

