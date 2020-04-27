#pragma once

#include "system.h"
#include "StdAfx.h"

#include "RenderData.h"

class CRenderTarget
{
public:
    CRenderTarget();
    CRenderTarget(UINT w, UINT h, BOOL depth=FALSE);

    void Release(void);

    inline LPDIRECT3DTEXTURE9 GetTextureHandle() { return mTextureHandle; }
    inline LPDIRECT3DSURFACE9 GetSurfaceHandle() { return mSurfaceHandle; }

    inline LPDIRECT3DTEXTURE9 GetDepthTextureHandle() { return mDepthTextureHandle; }
    inline LPDIRECT3DSURFACE9 GetDepthSurfaceHandle() { return mDepthSurfaceHandle; }

    void Bind();
    
private:
    BOOL mDepth;
    LPDIRECT3DTEXTURE9 mTextureHandle, mDepthTextureHandle;
    LPDIRECT3DSURFACE9 mSurfaceHandle, mDepthSurfaceHandle;

    void CreateRenderTarget(UINT w, UINT h, BOOL depth=FALSE);
};

