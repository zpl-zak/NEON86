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
    void ResetDevice();
    auto Release() -> bool;
    void Resize(RECT res);
    void SetVSYNC(bool state);
    void Blit();
    void BeginRender();
    void EndRender();

    /// Render commands
    void DrawMesh(const RENDERDATA& data);
    void DrawQuad(float x1, float x2, float y1, float y2, DWORD color, bool flipY = FALSE);
    void DrawQuadEx(float x, float y, float z, float w, float h, DWORD color, bool usesDepth, bool flipY = FALSE);
    void DrawQuad3D(float x1, float x2, float y1, float y2, float z1, float z2, DWORD color);
    void DrawPolygon(VERTEX& a, VERTEX& b, VERTEX& c);
    void DrawBox(D3DXMATRIX mat, D3DXVECTOR4 dims, DWORD color);
    void ClearBuffer(D3DCOLOR color, unsigned int flags = CLEARFLAG_STANDARD);
    void SetMaterial(DWORD stage, CMaterial* mat);
    void SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 handle);
    void SetMatrix(unsigned int matrixKind, const D3DXMATRIX& mat);
    void ResetMatrices();
    void SetRenderTarget(CRenderTarget* target);
    void SetRenderState(DWORD kind, DWORD value);
    void SetSamplerState(DWORD stage, DWORD kind, DWORD value);
    void SetFog(DWORD color, DWORD mode, float start, float end = 0.0F);
    void ClearFog();
    auto GetSurfaceResolution() -> RECT;

    void EnableLighting(bool state) { mEnableLighting = state; }
    auto GetLightingState() const -> bool { return mEnableLighting; }
    void SetActiveEffect(CEffect* fx) { mActiveEffect = fx; }

    void SetDefaultRenderStates();

    /// Accessors
    auto GetDevice() const -> LPDIRECT3DDEVICE9 { return mDevice; }
    auto GetResolution() const -> RECT { return mLastRes; }
    auto GetLocalCoordinates() const -> RECT;
    auto GetDeviceMatrix(unsigned int kind) -> D3DMATRIX;
    auto GetWindow() const -> HWND { return mWindow; }
    auto GetActiveEffect() const -> CEffect* { return mActiveEffect; }
    auto GetDisplayDesc() const -> D3DSURFACE_DESC { return mDisplayDesc; }
    auto IsLightingEnabled() const -> bool { return mEnableLighting; }
    auto IsFocused() const -> bool { return GetActiveWindow() == mWindow; }
    auto GetDefaultMaterial() const -> CMaterial* { return mDefaultMaterial; }
    auto UsesMaterialOverride() const -> bool { return mUsesMaterialOverride; }
    void MarkMaterialOverride(bool state) { mUsesMaterialOverride = state; }

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
    bool mUsesMaterialOverride;

    D3DSURFACE_DESC mDisplayDesc;
    bool mVsync;
    bool mFullscreen;
    bool mEnableLighting;
    VERTEX mImmediateBuffer[3];

    void Clear();
    void BuildParams();
    void PrepareEffectDraw();
};
