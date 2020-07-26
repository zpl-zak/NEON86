#pragma once

#include "system.h"
#include "StdAfx.h"
#include "ReferenceManager.h"

#include "RenderData.h"

class CRenderTarget: public CAllocable<CRenderTarget>, public CReferenceCounter
{
public:
    CRenderTarget();
    CRenderTarget(UINT w, UINT h, BOOL depth=FALSE);
    ~CRenderTarget() { Release(); }

    VOID Release(VOID);

    inline LPDIRECT3DTEXTURE9 GetTextureHandle() { return mTextureHandle; }
    inline LPDIRECT3DSURFACE9 GetSurfaceHandle() { return mSurfaceHandle; }

    inline LPDIRECT3DSURFACE9 GetDepthStencilSurfaceHandle() { return mDepthStencilSurfaceHandle; }
    inline BOOL HasDepth() { return mDepth; }

    VOID Bind(VOID);

private:
    BOOL mDepth;
    LPDIRECT3DTEXTURE9 mTextureHandle;
    LPDIRECT3DSURFACE9 mSurfaceHandle, mDepthStencilSurfaceHandle;

    VOID CreateRenderTarget(UINT w, UINT h, BOOL depth=FALSE);
};

