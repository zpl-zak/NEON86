#pragma once

#include "system.h"
#include "StdAfx.h"

#include "RenderData.h"

class CRenderTarget
{
public:
    CRenderTarget();
    CRenderTarget(UINT w, UINT h);

    void Release(void);

    inline LPDIRECT3DTEXTURE9 GetTextureHandle() { return mTextureHandle; }
    inline LPDIRECT3DTEXTURE9 GetDepthTextureHandle() { return mTextureHandle; }
    inline LPDIRECT3DSURFACE9 GetSurfaceHandle() { return mSurfaceHandle; }
    inline LPDIRECT3DSURFACE9 GetDepthHandle() { return mDepthHandle; }

    void Bind();
    
private:
    LPDIRECT3DTEXTURE9 mTextureHandle, mDepthTextureHandle;
    LPDIRECT3DSURFACE9 mSurfaceHandle, mDepthHandle;

    void CreateRenderTarget(UINT w, UINT h);
};

