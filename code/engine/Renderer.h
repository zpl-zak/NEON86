#pragma once

#include "system.h"

#pragma comment (lib, "d3d9.lib")

#define RENDERER CEngine::the()->GetRenderer()

class CFrustum;
class CEffect;
class CRenderTarget;
class CMaterial;

#include "RenderData.h"

class ENGINE_API CRenderer
{
public:
    CRenderer();

    auto CreateDevice(HWND window, RECT winres) -> LRESULT;
    VOID ResetDevice(VOID);
    auto Release(VOID) -> BOOL;
    VOID Resize(RECT res);
    VOID SetVSYNC(BOOL state);
    VOID Blit(VOID);
    VOID BeginRender(VOID);
    VOID EndRender(VOID);

    /// Render commands
    VOID DrawMesh(const RENDERDATA& data);
    VOID DrawQuad(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, DWORD color, BOOL flipY = FALSE);
    VOID DrawQuadEx(FLOAT x, FLOAT y, FLOAT z, FLOAT w, FLOAT h, DWORD color, BOOL usesDepth, BOOL flipY = FALSE);
    VOID DrawQuad3D(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, FLOAT z1, FLOAT z2, DWORD color);
    VOID DrawPolygon(VERTEX& a, VERTEX& b, VERTEX& c);
    VOID DrawBox(D3DXMATRIX mat, D3DXVECTOR4 dims, DWORD color);
    VOID ClearBuffer(D3DCOLOR color, UINT flags = CLEARFLAG_STANDARD);
    VOID SetMaterial(DWORD stage, CMaterial* mat);
    VOID SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 handle);
    VOID SetMatrix(UINT matrixKind, const D3DXMATRIX& mat);
    VOID ResetMatrices();
    VOID SetRenderTarget(CRenderTarget* target);
    VOID SetRenderState(DWORD kind, DWORD value);
    VOID SetSamplerState(DWORD stage, DWORD kind, DWORD value);
    VOID SetFog(DWORD color, DWORD mode, FLOAT start, FLOAT end = 0.0f);
    VOID ClearFog();
    auto GetSurfaceResolution() -> RECT;

    VOID EnableLighting(BOOL state) { mEnableLighting = state; }
    auto GetLightingState() const -> BOOL { return mEnableLighting; }
    VOID SetActiveEffect(CEffect* fx) { mActiveEffect = fx; }

    VOID SetDefaultRenderStates();

    /// Accessors
    auto GetDevice() const -> LPDIRECT3DDEVICE9 { return mDevice; }
    auto GetResolution() const -> RECT { return mLastRes; }
    auto GetLocalCoordinates(VOID) const -> RECT;
    auto GetDeviceMatrix(UINT kind) -> D3DMATRIX;
    auto GetWindow() const -> HWND { return mWindow; }
    auto GetActiveEffect() const -> CEffect* { return mActiveEffect; }
    auto GetDisplayDesc() const -> D3DSURFACE_DESC { return mDisplayDesc; }
    auto IsLightingEnabled() const -> BOOL { return mEnableLighting; }
    auto IsFocused() const -> BOOL { return GetActiveWindow() == mWindow; }
    auto GetDefaultMaterial() const -> CMaterial* { return mDefaultMaterial; }
    auto UsesMaterialOverride() const -> BOOL { return mUsesMaterialOverride; }
    VOID MarkMaterialOverride(BOOL state) { mUsesMaterialOverride = state; }

protected:
    LPDIRECT3D9 mDirect9;
    LPDIRECT3DDEVICE9 mDevice;
    D3DPRESENT_PARAMETERS mParams;
    RECT mLastRes;
    HWND mWindow;
    CEffect* mActiveEffect;
    CRenderTarget* mMainTarget;
    CRenderTarget* mActiveTarget;
    CMaterial* mDefaultMaterial;
    LPD3DXMESH mDefaultBox;
    BOOL mUsesMaterialOverride;

    D3DSURFACE_DESC mDisplayDesc;
    BOOL mVsync;
    BOOL mFullscreen;
    BOOL mEnableLighting;
    VERTEX mImmediateBuffer[3];

    VOID Clear(VOID);
    VOID BuildParams(VOID);
    VOID PrepareEffectDraw();
};
