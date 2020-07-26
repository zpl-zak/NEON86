#include "StdAfx.h"

#include "RenderTarget.h"

#include "Engine.h"
#include "Renderer.h"

CRenderTarget::CRenderTarget(): CAllocable()
{
    CreateRenderTarget(0,0, RTKIND_COLOR);
}

CRenderTarget::CRenderTarget(UINT w, UINT h, UCHAR kind): CAllocable()
{
    CreateRenderTarget(w, h, kind);
}

VOID CRenderTarget::Release(VOID)
{
    if (DelRef())
    {
        LPDIRECT3DSURFACE9 surf;
        RENDERER->GetDevice()->GetRenderTarget(0, &surf);

        if (surf == mSurfaceHandle)
            RENDERER->SetRenderTarget(NULL);

        SAFE_RELEASE(mTextureHandle);
        SAFE_RELEASE(mSurfaceHandle);
        SAFE_RELEASE(mDepthStencilSurfaceHandle);

        delete this;
    }
}

VOID CRenderTarget::Bind(VOID)
{
    RENDERER->SetRenderTarget(this);
}

VOID CRenderTarget::CreateRenderTarget(UINT w, UINT h, UCHAR kind)
{
    mKind = kind;
    mTextureHandle = NULL;
    mSurfaceHandle = NULL;
    mDepthStencilSurfaceHandle = NULL;

    D3DSURFACE_DESC dp = RENDERER->GetDisplayDesc();

    D3DFORMAT formats[] = {dp.Format, D3DFMT_R32F, D3DFMT_A16B16G16R16F, D3DFMT_A32B32G32R32F};

    LRESULT res = RENDERER->GetDevice()->CreateTexture(
        w ? w : dp.Width,
        h ? h : dp.Height,
        1,
        D3DUSAGE_RENDERTARGET,
        formats[kind],
        D3DPOOL_DEFAULT,
        &mTextureHandle,
        NULL
    );

    if (FAILED(res))
    {
        MessageBoxA(NULL, "Failed to create render target!", "Renderer error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    RENDERER->GetDevice()->CreateDepthStencilSurface(w, h, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &mDepthStencilSurfaceHandle, NULL);

    mTextureHandle->GetSurfaceLevel(0, &mSurfaceHandle);
}
