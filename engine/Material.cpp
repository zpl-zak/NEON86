#include "stdafx.h"
#include "Material.h"

#include "NeonEngine.h"

CMaterial::CMaterial(UINT slot, LPSTR texName)
{
    ZeroMemory(mTextureHandle, sizeof(mTextureHandle));
    ZeroMemory(mStats, sizeof(mStats));

    CreateTextureForSlot(slot, texName);
    DefaultMaterial();
}

CMaterial::CMaterial(UINT slot, UINT w, UINT h)
{
    CMaterial();

    CreateTextureForSlot(slot, NULL, w, h);
}

CMaterial::CMaterial()
{
    ZeroMemory(mTextureHandle, sizeof(mTextureHandle));
    ZeroMemory(mStats, sizeof(mStats));
    DefaultMaterial();
}

CMaterial::CMaterial(UINT slot, void* data, UINT size)
{
    CMaterial();

    CreateEmbeddedTextureForSlot(slot, data, size);
    DefaultMaterial();
}

void CMaterial::DefaultMaterial()
{
    ZeroMemory(&mMaterialData, sizeof(mMaterialData));

    mMaterialData.Ambient.r = mMaterialData.Ambient.g = mMaterialData.Ambient.b = mMaterialData.Ambient.a = 1.0f;
    mMaterialData.Specular.r = mMaterialData.Specular.g = mMaterialData.Specular.b = mMaterialData.Specular.a = 1.0f;
    mMaterialData.Diffuse.r = mMaterialData.Diffuse.g = mMaterialData.Diffuse.b = mMaterialData.Diffuse.a = 1.0f;
    mMaterialData.Opacity = 1.0f;
    mMaterialData.Power = 1.0f;
}

void CMaterial::Release(void)
{
    for (UINT i=0; i<MAX_TEXTURE_SLOTS;i++)
    {
        SAFE_RELEASE(mTextureHandle[i]);
    }
}

void CMaterial::CreateTextureForSlot(UINT slot, LPSTR texName, UINT w, UINT h)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();

    if (!texName)
        D3DXCreateTexture(dev, w, h, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mTextureHandle[slot]);
    else
    {
        FDATA img = FILESYSTEM->GetResource(RESOURCEKIND_IMAGE, texName);
        if (!img.data)
        {
            MessageBoxA(NULL, "Image not found!", "Resource error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
        D3DXCreateTextureFromFileInMemory(dev, img.data, img.size, &mTextureHandle[slot]);
    }
}

void CMaterial::CreateEmbeddedTextureForSlot(UINT slot, void* data, UINT size)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
    
    if (!data)
    {
        MessageBoxA(NULL, "Embedded image is empty!", "Texture error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    D3DXCreateTextureFromFileInMemory(dev, data, size, &mTextureHandle[slot]);
}

void CMaterial::SetUserTexture(UINT userSlot, LPDIRECT3DTEXTURE9 handle)
{
    if (userSlot >= (MAX_TEXTURE_SLOTS))
    {
        MessageBoxA(NULL, "User slot is invalid!", "Texture error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    mTextureHandle[userSlot] = handle;
}

void CMaterial::Bind(DWORD stage)
{
    RENDERER->SetMaterial(stage, this);

    /* Exit early since if this is a NULL material. */
    /* Exit if we use shaders as well. Shaders use their own sampler properties. */
    if (this == nullptr || RENDERER->GetActiveEffect())
        return;

    for (UINT i = 0; i < MAX_SAMPLER_STATES; i++)
    {
        RENDERER->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)i, mStats[i]);
    }
}

void CMaterial::Unbind(DWORD stage)
{
    RENDERER->SetMaterial(stage, NULL);
}

void* CMaterial::Lock(UINT slot)
{
    D3DLOCKED_RECT r;
    mTextureHandle[slot]->LockRect(0, &r, NULL, 0);
    return r.pBits;
}

void CMaterial::UploadARGB(UINT slot, void* data, UINT size)
{
    D3DLOCKED_RECT r;
    mTextureHandle[slot]->LockRect(0, &r, NULL, D3DLOCK_DISCARD);
    memcpy(r.pBits, data, size);
    mTextureHandle[slot]->UnlockRect(0);
}

void CMaterial::Unlock(UINT slot)
{
    mTextureHandle[slot]->UnlockRect(0);
}

void CMaterial::SetAmbient(D3DCOLORVALUE color)
{
    memcpy(&mMaterialData.Ambient, &color, sizeof(D3DCOLORVALUE));
}

void CMaterial::SetDiffuse(D3DCOLORVALUE color)
{
    memcpy(&mMaterialData.Diffuse, &color, sizeof(D3DCOLORVALUE));
}

void CMaterial::SetSpecular(D3DCOLORVALUE color)
{
    memcpy(&mMaterialData.Specular, &color, sizeof(D3DCOLORVALUE));
}

void CMaterial::SetEmission(D3DCOLORVALUE color)
{
    memcpy(&mMaterialData.Emissive, &color, sizeof(D3DCOLORVALUE));
}

void CMaterial::SetPower(FLOAT val)
{
    mMaterialData.Power = val;
}

void CMaterial::SetOpacity(FLOAT val)
{
    mMaterialData.Opacity = val;
}
