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

// 생성자
D3DMain::D3DMain(HINSTANCE hInstance)
//Window 관련 변수 초기화
	:m_hInstance(hInstance),
	m_hWindow(0),
	m_WindowCaption(L"D3D11 Application"),
	m_WindowWidth(600),
	m_WindowHeight(400),
	m_isPaused(false),
	m_isMinimized(false),
	m_isMaximized(false),
	m_isResizing(false),

	//DirectX 관련 변수 초기화
	m_D3_Device(0),
	m_D3_DeviceContext(0),
	m_SwapChain(0),
	m_DepthStencilBuffer(0),
	m_DepthStencilView(0),
	m_RenderTargetView(0),
	m_4XMSAA_quality(0),
	m_Enable4XMSAA(false),

	//기타 변수 초기화
	m_CurrentFramePerSeconds(0.0f)
{
	ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));
	gd3dApp = this;
}
//소멸자
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
// -- 프레임워크 메소드 --
//초기화
bool D3DMain::Init()
{
	if (!MakeWindow())
		return false;
	if (!InitDirectX3D())
		return false;

	return true;
}
//창 크기 변경
void D3DMain::OnResize()
{
	//다시 드로우
	ReDraw();
}
//화면 업데이트
void D3DMain::UpdateScene(float deltaTime)
{

}
//화면 그리기
void D3DMain::DrawScene()
{
}
//메시지 처리기
LRESULT D3DMain::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE) { // 창 활성화
			m_isPaused = true;
			m_GameTimer.Stop();
		}
		else { // 창 비활성화
			m_isPaused = false;
			m_GameTimer.Start();
		}
		return 0;

	case WM_SIZE: // 창 크기 변경
		return 0;
	case WM_ENTERSIZEMOVE: // 창 크기 조절 시작
		m_isPaused = true;
		m_isResizing = true;
		m_GameTimer.Stop();
		return 0;
	case WM_EXITSIZEMOVE: // 창 크기 조절 끝
		m_isPaused = false;
		m_isResizing = false;
		m_GameTimer.Start();
		OnResize();
		return 0;

	case WM_DESTROY: // 창 닫음
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_GETMINMAXINFO: // 창 최대/최소 설정
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

		// 마우스 이벤트
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

// -- 기타 메소드 --
//실행
int D3DMain::Run()
{
	MSG msg = { 0 };
	//초기 타이머 리셋
	m_GameTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		//메시지가 있으면 실행
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else//아닐경우 시간 실행
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
//인스턴스 핸들 변환
HINSTANCE D3DMain::GetInstanceHandle()const
{
	return m_hInstance;
}
//윈도우 핸들 변환
HWND D3DMain::GetWindowHandle()const
{
	return m_hWindow;
}
//종횡비 화면 비율 변환
float D3DMain::GetAspectRatio()const
{
	return static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
}
//FPS변환
float D3DMain::GetCurrentFramePerSeconds()const
{
	return m_CurrentFramePerSeconds;
}

//윈도우 생성
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

	//클라이언트 구역의 크기에 기반한 윈도우 사각형 크기를 계산
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
//다이렉트X 초기화
bool D3DMain::InitDirectX3D()
{
	//외장그래픽 
	IDXGIFactory* factory;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&factory));
	IDXGIAdapter* adapter;
	factory->EnumAdapters(1, &adapter);

	//DirectX 장치 생성
	UINT flag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL d3_featureLevel;
	HRESULT res = D3D11CreateDevice(
		adapter, // 디스플레이 어뎁터
		D3D_DRIVER_TYPE_UNKNOWN, // 드라이버 타입
		0, // 소프트웨어 구동기
		flag, // 플래그
		0, // 기능 수준 배열(순서대로)
		0, // 기능 수준 배열 크기
		D3D11_SDK_VERSION, // SDK 버전
		&m_D3_Device, // 장치 변환
		&d3_featureLevel, // 지원되는 기능 수준 반환
		&m_D3_DeviceContext // 장치 문맥 반환
		);

	if (FAILED(res)) // 장치 생성 실패
	{
		MessageBox(0, L"DirectX 11 장치 생성에 실패했습니다.", 0, 0);
		return false;
	}

	if (d3_featureLevel != D3D_FEATURE_LEVEL_11_0) //DirectX 11 지원 안함
	{
		MessageBox(0, L"이 장치는 DirectX 11을 지원하지 않습니다.", 0, 0);
		return false;
	}

	//4X MSAA 지원 여부 점검
	HR(m_D3_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4XMSAA_quality));
	assert(m_4XMSAA_quality > 0);

	//교환 사슬 설정
	DXGI_SWAP_CHAIN_DESC swapChainDescribe;
	//후면 버퍼 설정
	swapChainDescribe.BufferDesc.Width = m_WindowWidth; // 후면 버퍼 너비
	swapChainDescribe.BufferDesc.Height = m_WindowHeight; // 후면 버퍼 높이
	swapChainDescribe.BufferDesc.RefreshRate.Numerator = 60; // 갱신율(분자)
	swapChainDescribe.BufferDesc.RefreshRate.Denominator = 1; //갱신율(분모)
	swapChainDescribe.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 리소스 형식 지정
	swapChainDescribe.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 스캔라인 모드 설정
	swapChainDescribe.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 비례 모드 설정

																		  //다중표본화설정
																		  //4X MSAA을 사용할 경우 4배로 적용
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

	//기타 설정
	swapChainDescribe.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 버퍼 사용 용도(렌더링용)
	swapChainDescribe.BufferCount = 1; // 후면 버퍼 개수
	swapChainDescribe.OutputWindow = m_hWindow; // 결과를 표시할 윈도우
	swapChainDescribe.Windowed = true; // 창모드 설정
	swapChainDescribe.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 교환 효과 설정
	swapChainDescribe.Flags = 0; // 기타 플래그

								 //교환 사슬 생성
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

//--- 실행중 ---
//다시 그림
bool D3DMain::ReDraw()
{
	assert(m_D3_DeviceContext);
	assert(m_D3_Device);
	assert(m_SwapChain);

	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_DepthStencilBuffer);
	// 크기에 맞게 교환 사슬 / Render Target View 재설정
	HR(m_SwapChain->ResizeBuffers(1, m_WindowWidth, m_WindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_D3_Device->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView));
	ReleaseCOM(backBuffer);

	// 깊이/스텐실 뷰 설정(텍스쳐)
	D3D11_TEXTURE2D_DESC depthStencilDescribe;
	depthStencilDescribe.Width = m_WindowWidth; // 텍스쳐 너비
	depthStencilDescribe.Height = m_WindowHeight; // 텍스쳐 높이
	depthStencilDescribe.MipLevels = 1; // 밉맵 수준
	depthStencilDescribe.ArraySize = 1; // 텍스쳐 개수
	depthStencilDescribe.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 텍스쳐 형식

	 // 다중 표본화 설정
	 //4X MSAA를 사용할 경우 4배로 적용
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
	depthStencilDescribe.Usage = D3D11_USAGE_DEFAULT;  // 사용 용도 ( DEFAULT )
	depthStencilDescribe.BindFlags = D3D11_BIND_DEPTH_STENCIL; // 파이프라인에 묶이는 방법
	depthStencilDescribe.CPUAccessFlags = 0; // CPU 접근 설정
	depthStencilDescribe.MiscFlags = 0; // 기타 플래그

										// 깊이_스텐실 뷰 생성
	HR(m_D3_Device->CreateTexture2D(
		&depthStencilDescribe, // 설정
		0, // 초기 자료
		&m_DepthStencilBuffer // 버퍼 반환
		));
	HR(m_D3_Device->CreateDepthStencilView(
		m_DepthStencilBuffer, //깊이 버퍼
		0, // 설정
		&m_DepthStencilView // 뷰 반환
		));

	// 출력 병합기 단계에 묶기
	m_D3_DeviceContext->OMSetRenderTargets(
		1, // 렌더 개수
		&m_RenderTargetView, // 렌더 뷰 배열 포인터 
		m_DepthStencilView // 깊이_스텐실 뷰
		);

	
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.Width = static_cast<float>(m_WindowWidth);
	viewPort.Height = static_cast<float>(m_WindowHeight);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	m_D3_DeviceContext->RSSetViewports(
		1,  //뷰포트 개수
		&viewPort //뷰포트 배열 포인터 
		);

	return true;
}
//프레임 상태 계산
void D3DMain::CalculateFrameStats()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	// 1초가 지난 프레임 수 계산
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

		//초기화
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

