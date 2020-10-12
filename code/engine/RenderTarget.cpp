#include "StdAfx.h"

#include "RenderTarget.h"

#include "Engine.h"
#include "Renderer.h"

CRenderTarget::CRenderTarget()
{
    CreateRenderTarget(0, 0, RTKIND_COLOR);
}

CRenderTarget::CRenderTarget(unsigned int w, unsigned int h, UCHAR kind)
{
    CreateRenderTarget(w, h, kind);
}

CRenderTarget::~CRenderTarget()
{
    Release();
}

void CRenderTarget::Release()
{
    if (DelRef())
    {
        LPDIRECT3DSURFACE9 surf;
        RENDERER->GetDevice()->GetRenderTarget(0, &surf);

        if (surf == mSurfaceHandle)
        {
            RENDERER->SetRenderTarget(nullptr);
        }

        SAFE_RELEASE(mTextureHandle);
        SAFE_RELEASE(mSurfaceHandle);
        SAFE_RELEASE(mDepthStencilSurfaceHandle);

        delete this;
    }
}

void CRenderTarget::Bind()
{
    RENDERER->SetRenderTarget(this);
}

void CRenderTarget::CreateRenderTarget(unsigned int w, unsigned int h, UCHAR kind)
{
    mKind = kind;
    mTextureHandle = nullptr;
    mSurfaceHandle = nullptr;
    mDepthStencilSurfaceHandle = nullptr;

    D3DSURFACE_DESC dp = RENDERER->GetDisplayDesc();

    D3DFORMAT formats[] = {dp.Format, D3DFMT_R32F, D3DFMT_A16B16G16R16F, D3DFMT_A32B32G32R32F};

    LRESULT res = RENDERER->GetDevice()->CreateTexture(
        w != 0u ? w : dp.Width,
        h != 0u ? h : dp.Height,
        1,
        D3DUSAGE_RENDERTARGET,
        formats[kind],
        D3DPOOL_DEFAULT,
        &mTextureHandle,
        nullptr
    );

    if (FAILED(res))
    {
        MessageBoxA(nullptr, "Failed to create render target!", "Renderer error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    if (kind != RTKIND_DEPTH)
    {
        RENDERER->GetDevice()->CreateDepthStencilSurface(w, h, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE,
                                                         &mDepthStencilSurfaceHandle, nullptr);
    }

    mTextureHandle->GetSurfaceLevel(0, &mSurfaceHandle);
}
