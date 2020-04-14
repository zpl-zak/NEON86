#pragma once

#include "system.h"

#pragma comment (lib, "d3d9.lib")

#include <d3d9.h>
#include <d3dx9.h>

#define RENDERER CEngine::the()->GetRenderer()

class CRenderQueue;

#include "RenderData.h"

class ENGINE_API CRenderer  
{
public:
	CRenderer();

	LRESULT CreateDevice(HWND window);
	VOID ResetDevice(void);
	BOOL Release();
	VOID Present();
	VOID Resize(RECT res);
	VOID SetVSYNC(BOOL state);
	VOID AllowCommands(BOOL state);

	/// Render commands
	inline VOID PushCommand(UINT commandKind, const RENDERDATA& data);
	VOID PushClear(D3DCOLOR color, UINT flags=CLEARFLAG_STANDARD);
	VOID PushTexture(DWORD stage, LPDIRECT3DTEXTURE9 tex);
	VOID PushMatrix(UINT matrixKind, const D3DXMATRIX& mat);
	VOID PushRenderState(DWORD kind, BOOL state);

	/// Accessors
	inline LPDIRECT3DDEVICE9 GetDevice() { return mDevice; }
	inline RECT GetResolution() { return mLastRes; }
	D3DMATRIX GetDeviceMatrix(UINT kind);
	inline HWND GetWindow() const { return mWindow; }

protected:
	LPDIRECT3D9 mDirect9;
	LPDIRECT3DDEVICE9 mDevice;
	D3DPRESENT_PARAMETERS mParams;
	RECT mLastRes;
	HWND mWindow;
	CRenderQueue *mRenderQueue;

	BOOL mVsync;
	BOOL mFullscreen;
	BOOL mCanAddCommands;

	VOID Clear(void);
	VOID BuildParams(void);
};
