#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include "framework.h"
#include "resource.h"
#include "Main.h"
using namespace std;

/*
** DirectX Class
*/
class DirectX {

private:

	IDirect3D9* pDirect3D;
	IDirect3DDevice9* pDevice;
	IDirect3DSurface9* pBackBuffer = NULL;

public:

	D3DLOCKED_RECT      rect;
	HWND                hWnd;

	void Init(HWND hWndArgument) {
		// Save Window Handle
		hWnd = hWndArgument;

		// Direct3D initialize
		pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		pDirect3D->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
			&d3dpp, &this->pDevice
		);
	}

	void ClearBackBuffer()
	{
		pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);
	}

	void LockBackBuffer() {
		this->pDevice->GetBackBuffer(
			0,
			0,
			D3DBACKBUFFER_TYPE_MONO,
			&pBackBuffer);

		pBackBuffer->LockRect(&rect, NULL, NULL);
	}

	void UnlockBackBuffer() {
		pBackBuffer->UnlockRect();
		pBackBuffer->Release();
	}

	void Back2FrontBuffer()
	{
		pDevice->Present(NULL, NULL, NULL, NULL);
	}

	void Destroy() {
		if (pDevice) {
			pDevice->Release();
			pDevice = NULL;
		}
		if (pDirect3D) {
			pDirect3D->Release();
			pDirect3D = NULL;
		}
	}
};



/*
** Definations
*/
#define WindowClassName   L"FrameBuffer"
#define WindowTitle       L"FrameBuffer"
#define ClearBufferWhenLockingBackBuffer FALSE


/*
** Global Variables
*/
int WindowLeftMargin;
int WindowTopMargin;
int WindowWidth;
int WindowHeight;
DirectX dx;
BOOL FirstTimeRunning = TRUE;


/*
** Functions
*/

void GetScreenResolution(int* resultX, int* resultY) {
	// Get Screen HDC
	HDC hdcScreen;
	hdcScreen = CreateDC(L"DISPLAY", NULL, NULL, NULL);

	// Get X and Y
	*resultX = GetDeviceCaps(hdcScreen, HORZRES);    // pixel
	*resultY = GetDeviceCaps(hdcScreen, VERTRES);    // pixel

	// Release HDC
	if (NULL != hdcScreen)
	{
		DeleteDC(hdcScreen);
	}
}


/*
** Message Processing Function
*/
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


/*
** Main Function
*/
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{
	/*
	** Calculate Window Width And Height
	*/
	int ScreenX, ScreenY;
	GetScreenResolution(&ScreenX, &ScreenY);

	// Only 95% of Height are Available, others are Task Bar
	ScreenY = (float)ScreenY * (float)0.95;

	int Unit = ScreenY / 30;

	WindowTopMargin  = 1 * Unit;
	WindowHeight     = 28 * Unit;

	WindowLeftMargin = Unit;
	WindowWidth      = ScreenX - 2 * Unit;


	/*
	** Register Window Class
	*/

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0, 0,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		WindowClassName, NULL
	};

	wc.hIconSm = (HICON)LoadImage(
		hInst, MAKEINTRESOURCE(NULL), IMAGE_ICON, 16, 16, 0
	);

	wc.hIcon = (HICON)LoadImage(
		hInst, MAKEINTRESOURCE(NULL), IMAGE_ICON, 32, 32, 0
	);

	RegisterClassEx(&wc);


	/*
	** Create Window
	*/

	// Create Window Rect
	RECT wr;
	wr.left = WindowLeftMargin;
	wr.top = WindowTopMargin;
	wr.right = WindowLeftMargin + WindowWidth;
	wr.bottom = WindowTopMargin + WindowHeight;
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	// Create Window
	HWND hWnd = CreateWindowW(
		WindowClassName, WindowTitle, WS_OVERLAPPEDWINDOW,
		WindowLeftMargin, WindowTopMargin,
		WindowWidth, WindowHeight,
		NULL, NULL, wc.hInstance, NULL
	);

	// Show And Update Window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	// Initialize FrameBuffer
	dx.Init(hWnd);


	/*
	** Messages Loop
	*/

	// Create Message Variable
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	// While Window Exist
	while (msg.message != WM_QUIT)
	{
		// If There Is A New Message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Deal With It
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If There Is No Message
		else
		{
			/*
			** Main Loop!
			*/

			// If you want to clear the buffer
			if (ClearBufferWhenLockingBackBuffer) {
				// Clear it
				dx.ClearBackBuffer();
			}

			// Lock the Back Buffer
			dx.LockBackBuffer();

			// Main Function!
			if (FirstTimeRunning) {
				Setup(dx.rect, WindowWidth, WindowHeight);
				FirstTimeRunning = FALSE;
			}
			else {
				Update(dx.rect, WindowWidth, WindowHeight);
			}

			// Unlock the Back Buffer
			dx.UnlockBackBuffer();

			// Copy Back Buffer to Front Buffer
			dx.Back2FrontBuffer();
		}
	}

	// When WM_DESTROY, Release All the Variables
	UnregisterClass(WindowClassName, wc.hInstance);
	OnDestroy(dx.rect, WindowWidth, WindowHeight);
	dx.Destroy();

	return 0;
}
