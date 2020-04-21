#include "stdafx.h"
#include "Material.h"

#include "NeonEngine.h"

CMaterial::CMaterial(LPSTR texName, UINT w, UINT h)
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

    DefaultMaterial();
}

CMaterial::CMaterial(VOID* data, UINT size)
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

    DefaultMaterial();
}

VOID CMaterial::DefaultMaterial()
{
    ZeroMemory(&mMaterialData, sizeof(mMaterialData));

    mMaterialData.Ambient.r = mMaterialData.Ambient.g = mMaterialData.Ambient.b = mMaterialData.Ambient.a = 1.0f;
    mMaterialData.Specular.r = mMaterialData.Specular.g = mMaterialData.Specular.b = mMaterialData.Specular.a = 1.0f;
    mMaterialData.Diffuse.r = mMaterialData.Diffuse.g = mMaterialData.Diffuse.b = mMaterialData.Diffuse.a = 1.0f;
    mMaterialData.Opacity = 1.0f;
    mMaterialData.Power = 1.0f;
}

VOID CMaterial::Release(void)
{
    SAFE_RELEASE(mTextureHandle);
}

VOID CMaterial::Bind(DWORD stage)
{
    RENDERER->SetMaterial(stage, this);

    for (UINT i = 0; i < MAX_SAMPLER_STATES; i++)
    {
        RENDERER->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)i, mStats[i]);
    }
}

VOID CMaterial::Unbind(DWORD stage)
{
    RENDERER->SetMaterial(stage, NULL);
}

VOID* CMaterial::Lock()
{
    D3DLOCKED_RECT r;
    mTextureHandle->LockRect(0, &r, NULL, 0);
    return r.pBits;
}

VOID CMaterial::UploadRGB888(VOID* data, UINT size)
{
    D3DLOCKED_RECT r;
    mTextureHandle->LockRect(0, &r, NULL, D3DLOCK_DISCARD);
    memcpy(r.pBits, data, size);
    mTextureHandle->UnlockRect(0);
}

VOID CMaterial::Unlock()
{
    mTextureHandle->UnlockRect(0);
}

VOID CMaterial::SetAmbient(D3DCOLORVALUE color)
{
    memcpy(&mMaterialData.Ambient, &color, sizeof(D3DCOLORVALUE));
}

VOID CMaterial::SetDiffuse(D3DCOLORVALUE color)
{
    memcpy(&mMaterialData.Diffuse, &color, sizeof(D3DCOLORVALUE));
}

VOID CMaterial::SetSpecular(D3DCOLORVALUE color)
{
    memcpy(&mMaterialData.Specular, &color, sizeof(D3DCOLORVALUE));
}

VOID CMaterial::SetEmission(D3DCOLORVALUE color)
{
    memcpy(&mMaterialData.Emissive, &color, sizeof(D3DCOLORVALUE));
}

VOID CMaterial::SetPower(FLOAT val)
{
    mMaterialData.Power = val;
}

void CMaterial::SetOpacity(FLOAT val)
{
    mMaterialData.Opacity = val;
}
