#pragma once

#include "system.h"
#include "StdAfx.h"
#include "ReferenceManager.h"

#include "RenderData.h"

class CRenderTarget : public CAllocable<CRenderTarget>, public CReferenceCounter
{
public:
    CRenderTarget();
    CRenderTarget(UINT w, UINT h, UCHAR kind = RTKIND_COLOR);
    ~CRenderTarget() { Release(); }

    VOID Release(VOID);

    auto GetTextureHandle() const -> LPDIRECT3DTEXTURE9 { return mTextureHandle; }
    auto GetSurfaceHandle() const -> LPDIRECT3DSURFACE9 { return mSurfaceHandle; }

    auto GetDepthStencilSurfaceHandle() const -> LPDIRECT3DSURFACE9 { return mDepthStencilSurfaceHandle; }
    auto GetKind() const -> UCHAR { return mKind; }

    VOID Bind(VOID);

private:
    UCHAR mKind;
    LPDIRECT3DTEXTURE9 mTextureHandle;
    LPDIRECT3DSURFACE9 mSurfaceHandle, mDepthStencilSurfaceHandle;

    VOID CreateRenderTarget(UINT w, UINT h, UCHAR kind = RTKIND_COLOR);
};
