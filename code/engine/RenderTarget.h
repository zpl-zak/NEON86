#pragma once

#include "system.h"
#include "StdAfx.h"
#include "ReferenceManager.h"

#include "RenderData.h"

class CRenderTarget : public CAllocable<CRenderTarget>, public CReferenceCounter
{
public:
    CRenderTarget();
    CRenderTarget(unsigned int w, unsigned int h, UCHAR kind = RTKIND_COLOR);
    ~CRenderTarget() { Release(); }

    void Release(void);

    auto GetTextureHandle() const -> LPDIRECT3DTEXTURE9 { return mTextureHandle; }
    auto GetSurfaceHandle() const -> LPDIRECT3DSURFACE9 { return mSurfaceHandle; }

    auto GetDepthStencilSurfaceHandle() const -> LPDIRECT3DSURFACE9 { return mDepthStencilSurfaceHandle; }
    auto GetKind() const -> UCHAR { return mKind; }

    void Bind(void);

private:
    UCHAR mKind;
    LPDIRECT3DTEXTURE9 mTextureHandle;
    LPDIRECT3DSURFACE9 mSurfaceHandle, mDepthStencilSurfaceHandle;

    void CreateRenderTarget(unsigned int w, unsigned int h, UCHAR kind = RTKIND_COLOR);
};
