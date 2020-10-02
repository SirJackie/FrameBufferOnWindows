#pragma once

#include <d3d9.h>
#include <Windows.h>

#define Pixel(fb, x, y) ((D3DCOLOR*)(*fb).rect.pBits)[x + ((*fb).rect.Pitch >> 2) * y]
#define RGB888(r, g, b) (D3DCOLOR_XRGB(r, g, b))

class FrameBuffer {

private:

	IDirect3D9* pDirect3D;
	IDirect3DDevice9* pDevice;
	IDirect3DSurface9* pBackBuffer = NULL;

public:

	D3DLOCKED_RECT      rect;
	HWND                hWnd;

	void Init(HWND hWndArgument) {
		// Save Window Handle
		this->hWnd = hWndArgument;

		// Direct3D initialize
		this->pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		this->pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->hWnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE, &d3dpp, &this->pDevice);
	}

	void ClearBackBuffer()
	{
		this->pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);
	}

	void LockBackBuffer() {
		this->pDevice->GetBackBuffer(
			0,
			0,
			D3DBACKBUFFER_TYPE_MONO,
			&(this->pBackBuffer));

		this->pBackBuffer->LockRect(&(this->rect), NULL, NULL);
	}

	void UnlockBackBuffer() {
		this->pBackBuffer->UnlockRect();
		this->pBackBuffer->Release();
	}

	void Back2FrontBuffer()
	{
		this->pDevice->Present(NULL, NULL, NULL, NULL);
	}

	void Destroy() {
		if (this->pDevice) {
			this->pDevice->Release();
			this->pDevice = NULL;
		}
		if (this->pDirect3D) {
			this->pDirect3D->Release();
			this->pDirect3D = NULL;
		}
	}
};
