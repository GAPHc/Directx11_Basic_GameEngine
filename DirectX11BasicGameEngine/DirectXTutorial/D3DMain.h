#include"D3Utility.h"
#include"GameTimer.h"
#include<string>

class D3DMain
{
public:
	// constructor / destructor
	// ������/�Ҹ���
	D3DMain(HINSTANCE hInstance);
	virtual ~D3DMain();

	// Framework method, overriding
	// �����ӿ�ũ �޼ҵ�, �������̵�
	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float deltaTime);
	virtual void DrawScene();

	virtual GameTimer GetGameTimer();
	virtual ID3D11Device* GetDevice();
	virtual ID3D11DeviceContext* GetDeviceContext();

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

	// other methods
	// ��Ÿ �޼ҵ�
	int Run();
	HINSTANCE GetInstanceHandle()const;
	HWND GetWindowHandle()const;
	float GetAspectRatio()const;
	float GetCurrentFramePerSeconds()const;
	

protected:
	bool MakeWindow();
	bool InitDirectX3D();
	bool ReDraw();

protected:
	// Variable of window
	// Window ���� ����
	HINSTANCE m_hInstance;
	HWND m_hWindow;
	std::wstring m_WindowCaption;
	int m_WindowWidth;
	int m_WindowHeight;
	bool m_isPaused;
	bool m_isMinimized;
	bool m_isMaximized;
	bool m_isResizing;

	// Functions of DirectX
	// DirectX ���� �Լ�
	ID3D11Device* m_D3_Device;
	ID3D11DeviceContext* m_D3_DeviceContext;
	IDXGISwapChain* m_SwapChain;
	ID3D11Texture2D* m_DepthStencilBuffer;
	ID3D11DepthStencilView* m_DepthStencilView;
	ID3D11RenderTargetView* m_RenderTargetView;
	D3D11_VIEWPORT viewPort;
	UINT m_4XMSAA_quality;
	bool m_Enable4XMSAA;

	// Function or Variable of game state
	// ���� ���� �Լ�/����
	void CalculateFrameStats();
	GameTimer m_GameTimer;
	float m_CurrentFramePerSeconds;

	// type of driver
	// ����̹� Ÿ��(�ӽ�)
	//D3D_DRIVER_TYPE m_d3dDriverType;
};

