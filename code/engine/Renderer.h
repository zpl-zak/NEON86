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

    LRESULT CreateDevice(HWND window, RECT winres);
    VOID ResetDevice(VOID);
    BOOL Release(VOID);
    VOID Resize(RECT res);
    VOID SetVSYNC(BOOL state);
    VOID Blit(VOID);
    VOID BeginRender(VOID);
    VOID EndRender(VOID);

    /// Render commands
    VOID DrawMesh(const RENDERDATA& data);
    VOID DrawQuad(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, DWORD color, BOOL flipY = FALSE);
    VOID DrawQuad3D(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, FLOAT z1, FLOAT z2, DWORD color);
    VOID DrawPolygon(VERTEX& a, VERTEX& b, VERTEX& c);
    VOID DrawBox(D3DXMATRIX mat, D3DXVECTOR4 dims, DWORD color);
    VOID ClearBuffer(D3DCOLOR color, UINT flags = CLEARFLAG_STANDARD);
    VOID SetMaterial(DWORD stage, CMaterial* mat);
    VOID SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 handle);
    VOID SetMatrix(UINT matrixKind, const D3DXMATRIX& mat);
    VOID ResetMatrices();
    VOID SetRenderTarget(CRenderTarget* target, BOOL depth = FALSE);
    VOID SetRenderState(DWORD kind, DWORD value);
    VOID SetSamplerState(DWORD stage, DWORD kind, DWORD value);
    VOID SetFog(DWORD color, DWORD mode, FLOAT start, FLOAT end=0.0f);
    VOID ClearFog();
    
    inline VOID EnableLighting(BOOL state) { mEnableLighting = state; }
    inline BOOL GetLightingState() { return mEnableLighting; }
    inline VOID SetActiveEffect(CEffect* fx) { mActiveEffect = fx; }

    VOID SetDefaultRenderStates();

    /// Accessors
    inline LPDIRECT3DDEVICE9 GetDevice() { return mDevice; }
    inline RECT GetResolution() { return mLastRes; }
    RECT GetLocalCoordinates(VOID) const;
    D3DMATRIX GetDeviceMatrix(UINT kind);
    inline HWND GetWindow() const { return mWindow; }
    inline CEffect* GetActiveEffect() { return mActiveEffect; }
    inline D3DSURFACE_DESC GetDisplayDesc() { return mDisplayDesc; }
    inline BOOL IsLightingEnabled() { return mEnableLighting; }
    inline BOOL IsFocused() { return GetActiveWindow() == mWindow; }
    inline CMaterial* GetDefaultMaterial() { return mDefaultMaterial;  }
    inline BOOL UsesMaterialOverride() { return mUsesMaterialOverride; }
    inline VOID MarkMaterialOverride(BOOL state) { mUsesMaterialOverride = state; }

protected:
    LPDIRECT3D9 mDirect9;
    LPDIRECT3DDEVICE9 mDevice;
    D3DPRESENT_PARAMETERS mParams;
    RECT mLastRes;
    HWND mWindow;
    CEffect* mActiveEffect;
    CRenderTarget* mMainTarget;
    CMaterial *mDefaultMaterial;
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
