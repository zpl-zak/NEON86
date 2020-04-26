#include "StdAfx.h"

#include "RenderTarget.h"
#include "NeonEngine.h"

CRenderTarget::CRenderTarget()
{
    CreateRenderTarget(0,0);
}

CRenderTarget::CRenderTarget(UINT w, UINT h)
{
    CreateRenderTarget(w, h);
}

void CRenderTarget::Release(void)
{
    LPDIRECT3DSURFACE9 surf;
    RENDERER->GetDevice()->GetRenderTarget(0, &surf);

    if (surf == mSurfaceHandle)
        RENDERER->SetRenderTarget(NULL);
}

void CRenderTarget::Bind()
{
    RENDERER->SetRenderTarget(this);
}

void CRenderTarget::CreateRenderTarget(UINT w, UINT h)
{
    mTextureHandle = NULL;
    mSurfaceHandle = NULL;
    mDepthHandle = NULL;
    mDepthTextureHandle = NULL;

    D3DSURFACE_DESC dp = RENDERER->GetDisplayDesc();

    LRESULT res = RENDERER->GetDevice()->CreateTexture(
        w ? w : dp.Width,
        h ? h : dp.Height,
        0,
        D3DUSAGE_RENDERTARGET,
        dp.Format,
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

    /*res = RENDERER->GetDevice()->CreateTexture(
        w,
        h,
        0,
        D3DUSAGE_RENDERTARGET,
        D3DFMT_D24S8,
        D3DPOOL_DEFAULT,
        &mDepthTextureHandle,
        NULL
    );

    if (FAILED(res))
    {
        MessageBoxA(NULL, "Failed to create render target!", "Renderer error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    mDepthTextureHandle->GetSurfaceLevel(0, &mDepthHandle);*/

    mTextureHandle->GetSurfaceLevel(0, &mSurfaceHandle);
}
