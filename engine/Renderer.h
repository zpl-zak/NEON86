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
    VOID ResetDevice(VOID);
    BOOL Release();
    VOID Resize(RECT res);
    VOID SetVSYNC(BOOL state);
    VOID Blit();

    /// Render commands
    VOID DrawMesh(const RENDERDATA& data);
    VOID DrawQuad(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, DWORD color, BOOL flipY = FALSE);
    VOID ClearBuffer(D3DCOLOR color, UINT flags = CLEARFLAG_STANDARD);
    VOID SetMaterial(DWORD stage, CMaterial* mat);
    VOID SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 handle);
    VOID SetMatrix(UINT matrixKind, const D3DXMATRIX& mat);
    VOID ResetMatrices();
    VOID SetRenderTarget(CRenderTarget* target, BOOL depth = FALSE);
    VOID SetRenderState(DWORD kind, DWORD value);
    VOID SetSamplerState(DWORD stage, DWORD kind, DWORD value);

    inline VOID SetActiveEffect(CEffect* fx) { mActiveEffect = fx; }

    VOID SetDefaultRenderStates();

    /// Accessors
    inline LPDIRECT3DDEVICE9 GetDevice() { return mDevice; }
    inline RECT GetResolution() { return mLastRes; }
    RECT GetLocalCoordinates() const;
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

    VOID Clear(VOID);
    VOID BuildParams(VOID);
};
