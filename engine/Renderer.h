#pragma once

#include "system.h"

#pragma comment (lib, "d3d9.lib")

#include <d3d9.h>
#include <d3dx9.h>

#define RENDERER CEngine::the()->GetRenderer()

class CFrustum;

#include "RenderData.h"

class ENGINE_API CRenderer  
{
public:
	CRenderer();

	LRESULT CreateDevice(HWND window);
	VOID ResetDevice(void);
	BOOL Release();
	VOID Resize(RECT res);
	VOID SetVSYNC(BOOL state);
	
	/// Render commands
	VOID DrawMesh(const RENDERDATA& data);
	VOID ClearBuffer(D3DCOLOR color, UINT flags=CLEARFLAG_STANDARD);
	VOID SetTexture(DWORD stage, CMaterial* tex);
	VOID SetMatrix(UINT matrixKind, const D3DXMATRIX& mat);
	VOID SetRenderState(DWORD kind, DWORD value);
	VOID SetSamplerState(DWORD stage, DWORD kind, DWORD value);

	/// Accessors
	inline LPDIRECT3DDEVICE9 GetDevice() { return mDevice; }
	inline RECT GetResolution() { return mLastRes; }
	D3DMATRIX GetDeviceMatrix(UINT kind);
	inline HWND GetWindow() const { return mWindow; }
	inline CFrustum* GetFrustum() { return mFrustum; }

protected:
	LPDIRECT3D9 mDirect9;
	LPDIRECT3DDEVICE9 mDevice;
	D3DPRESENT_PARAMETERS mParams;
	RECT mLastRes;
	HWND mWindow;
	CFrustum* mFrustum;

	BOOL mVsync;
	BOOL mFullscreen;
	
	VOID Clear(void);
	VOID BuildParams(void);
};
