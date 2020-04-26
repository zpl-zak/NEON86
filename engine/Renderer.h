#pragma once

#include "system.h"

#pragma comment (lib, "d3d9.lib")

#include <d3d9.h>
#include <d3dx9.h>

#define RENDERER CEngine::the()->GetRenderer()

class CFrustum;
class CEffect;
class CRenderTarget;

#include "RenderData.h"

class ENGINE_API CRenderer  
{
public:
	CRenderer();

	LRESULT CreateDevice(HWND window, RECT winres);
	void ResetDevice(void);
	BOOL Release();
	void Resize(RECT res);
	void SetVSYNC(BOOL state);
	void Blit();
	
	/// Render commands
	void DrawMesh(const RENDERDATA& data);
	void DrawQuad(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, DWORD color);
	void ClearBuffer(D3DCOLOR color, UINT flags=CLEARFLAG_STANDARD);
	void SetMaterial(DWORD stage, CMaterial* tex);
	void SetMatrix(UINT matrixKind, const D3DXMATRIX& mat);
	void ResetMatrices();
	void SetRenderTarget(CRenderTarget* target);
	void SetRenderState(DWORD kind, DWORD value);
	void SetSamplerState(DWORD stage, DWORD kind, DWORD value);

	inline void SetActiveEffect(CEffect* fx) { mActiveEffect = fx; }

	void SetDefaultRenderStates();

	/// Accessors
	inline LPDIRECT3DDEVICE9 GetDevice() { return mDevice; }
	inline RECT GetResolution() { return mLastRes; }
	D3DMATRIX GetDeviceMatrix(UINT kind);
	inline HWND GetWindow() const { return mWindow; }
	inline CFrustum* GetFrustum() { return mFrustum; }
	inline CEffect* GetActiveEffect() { return mActiveEffect; }
	inline D3DSURFACE_DESC GetDisplayDesc() { return mDisplayDesc; }

protected:
	LPDIRECT3D9 mDirect9;
	LPDIRECT3DDEVICE9 mDevice;
	D3DPRESENT_PARAMETERS mParams;
	RECT mLastRes;
	HWND mWindow;
	CFrustum* mFrustum;
	CEffect* mActiveEffect;
	CRenderTarget* mMainTarget;
	
	D3DSURFACE_DESC mDisplayDesc;
	BOOL mVsync;
	BOOL mFullscreen;
	
	void Clear(void);
	void BuildParams(void);
};
