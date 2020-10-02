#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include "framework.h"
#include "resource.h"
#include "Main.h"
using namespace std;


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
FrameBuffer fb;
BOOL FirstTimeRunning = TRUE;


/*
** Function Declaration
*/
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


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
	fb.Init(hWnd);


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
				fb.ClearBackBuffer();
			}

			// Lock the Back Buffer
			fb.LockBackBuffer();

			// Main Function!
			if (FirstTimeRunning) {
				Setup(&fb, WindowWidth, WindowHeight);
				FirstTimeRunning = FALSE;
			}
			else {
				Update(&fb, WindowWidth, WindowHeight);
			}

			// Unlock the Back Buffer
			fb.UnlockBackBuffer();

			// Copy Back Buffer to Front Buffer
			fb.Back2FrontBuffer();
		}
	}

	// When WM_DESTROY, Release All the Variables
	UnregisterClass(WindowClassName, wc.hInstance);
	OnDestroy(&fb, WindowWidth, WindowHeight);
	fb.Destroy();

	return 0;
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
