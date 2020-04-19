#include "stdafx.h"
#include "Texture.h"

#include "NeonEngine.h"

CTexture::CTexture(LPSTR texName, UINT w, UINT h)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
    mTextureHandle = NULL;
    ZeroMemory(mStats, sizeof(mStats));

    if (!texName)
        D3DXCreateTexture(dev, w, h, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &mTextureHandle);
    else
    {
        FDATA img = FILESYSTEM->GetResource(RESOURCEKIND_IMAGE, texName);
        if (!img.data)
        {
            MessageBoxA(NULL, "Image not found!", "Resource error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
        D3DXCreateTextureFromFileInMemory(dev, img.data, img.size, &mTextureHandle);
    }
}

CTexture::CTexture(VOID* data, UINT size)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
    mTextureHandle = NULL;
    ZeroMemory(mStats, sizeof(mStats));

    if (!data)
    {
        MessageBoxA(NULL, "Embedded image is empty!", "Texture error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    D3DXCreateTextureFromFileInMemory(dev, data, size, &mTextureHandle);
}

VOID CTexture::Release(void)
{
    SAFE_RELEASE(mTextureHandle);
}

VOID CTexture::Bind(DWORD stage)
{
    RENDERER->SetTexture(stage, this);

    for (UINT i = 0; i < MAX_SAMPLER_STATES; i++)
    {
        RENDERER->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)(i + 1), mStats[i]);
    }
}

VOID CTexture::Unbind(DWORD stage)
{
    RENDERER->SetTexture(stage, NULL);
}

VOID* CTexture::Lock()
{
    D3DLOCKED_RECT r;
    mTextureHandle->LockRect(0, &r, NULL, 0);
    return r.pBits;
}

VOID CTexture::UploadRGB888(VOID* data, UINT size)
{
    D3DLOCKED_RECT r;
    mTextureHandle->LockRect(0, &r, NULL, D3DLOCK_DISCARD);
    memcpy(r.pBits, data, size);
    mTextureHandle->UnlockRect(0);
}

VOID CTexture::Unlock()
{
    mTextureHandle->UnlockRect(0);
}
