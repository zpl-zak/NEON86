#include "stdafx.h"
#include "Material.h"

#include "Engine.h"
#include "VM.h"
#include "Renderer.h"
#include "FileSystem.h"

CMaterial::CMaterial(UINT slot, LPSTR texName): CMaterial()
{
    CreateTextureForSlot(slot, texName);
}

CMaterial::CMaterial(UINT slot, UINT w, UINT h): CMaterial()
{
    CreateTextureForSlot(slot, NULL, w, h);
}

CMaterial::CMaterial(): CAllocable()
{
    ZeroMemory(mTextureHandle, sizeof(mTextureHandle));
    ZeroMemory(mStats, sizeof(mStats));
    DefaultMaterial();
}

CMaterial::CMaterial(UINT slot, VOID* data, UINT size): CMaterial()
{
    CreateEmbeddedTextureForSlot(slot, data, size);
}

VOID CMaterial::DefaultMaterial(VOID)
{
    ZeroMemory(&mMaterialData, sizeof(mMaterialData));

    mMaterialData.Ambient.r = mMaterialData.Ambient.g = mMaterialData.Ambient.b = mMaterialData.Ambient.a = 1.0f;
    mMaterialData.Specular.r = mMaterialData.Specular.g = mMaterialData.Specular.b = mMaterialData.Specular.a = 1.0f;
    mMaterialData.Diffuse.r = mMaterialData.Diffuse.g = mMaterialData.Diffuse.b = mMaterialData.Diffuse.a = 1.0f;
    mMaterialData.Opacity = 1.0f;
    mMaterialData.Power = 1.0f;
    mMaterialData.Shaded = TRUE;
    mMaterialData.AlphaRef = 127;

    SetSamplerState(D3DSAMP_MINFILTER, TEXF_ANISOTROPIC);
    SetSamplerState(D3DSAMP_MIPFILTER, TEXF_ANISOTROPIC);
    SetSamplerState(D3DSAMP_MAGFILTER, TEXF_ANISOTROPIC);
    SetSamplerState(D3DSAMP_MAXANISOTROPY, 16);
}

VOID CMaterial::Release(VOID)
{
    if (DelRef())
    {
        for (UINT i = 0; i < MAX_TEXTURE_SLOTS; i++)
        {
            SAFE_RELEASE(mTextureHandle[i]);
        }

        delete this;
    }
}

VOID CMaterial::CreateTextureForSlot(UINT slot, LPSTR texName, UINT w, UINT h)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();

    if (!texName)
        D3DXCreateTexture(dev, w, h, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mTextureHandle[slot]);
    else
    {
        FDATA img = FILESYSTEM->GetResource(RESOURCEKIND_IMAGE, texName);
        if (!img.data)
        {
            VM->PostError(CString("Image not found: %s", texName));
            return;
        }
        D3DXCreateTextureFromFileInMemory(dev, img.data, img.size, &mTextureHandle[slot]);
    }
}

VOID CMaterial::CreateEmbeddedTextureForSlot(UINT slot, VOID* data, UINT size)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
    
    if (!data)
    {
        VM->PostError(CString("Embedded image is empty!"));
        ENGINE->Shutdown();
        return;
    }

    D3DXCreateTextureFromFileInMemory(dev, data, size, &mTextureHandle[slot]);
}

VOID CMaterial::SetUserTexture(UINT userSlot, LPDIRECT3DTEXTURE9 handle)
{
    if (userSlot >= (MAX_TEXTURE_SLOTS))
    {
        VM->PostError("User slot is invalid!");
        return;
    }

    mTextureHandle[userSlot] = handle;
}

VOID CMaterial::Bind(DWORD stage)
{
    RENDERER->SetMaterial(stage, this);

    /* Exit early if this is a NULL material. */
    /* Exit if we use shaders as well. Shaders use their own sampler properties. */
    if (this == nullptr || RENDERER->GetActiveEffect())
        return;

    for (UINT i = 0; i < MAX_SAMPLER_STATES; i++)
    {
        RENDERER->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)i, mStats[i]);
    }
}

VOID CMaterial::Unbind(DWORD stage)
{
    RENDERER->SetMaterial(stage, NULL);
}

VOID* CMaterial::Lock(UINT slot)
{
    D3DLOCKED_RECT r;
    mTextureHandle[slot]->LockRect(0, &r, NULL, 0);
    return r.pBits;
}

VOID CMaterial::UploadARGB(UINT slot, VOID* data, UINT size)
{
    D3DLOCKED_RECT r;
    mTextureHandle[slot]->LockRect(0, &r, NULL, D3DLOCK_DISCARD);
    memcpy(r.pBits, data, size);
    mTextureHandle[slot]->UnlockRect(0);
}

VOID CMaterial::Unlock(UINT slot)
{
    mTextureHandle[slot]->UnlockRect(0);
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

VOID CMaterial::SetOpacity(FLOAT val)
{
    mMaterialData.Opacity = val;
}

VOID CMaterial::SetShaded(BOOL state)
{
    mMaterialData.Shaded = state;
}

VOID CMaterial::SetAlphaIsTransparency(BOOL state)
{
    mMaterialData.AlphaIsTransparency = state;
}

VOID CMaterial::SetEnableAlphaTest(BOOL state)
{
    mMaterialData.AlphaTestEnabled = TRUE;
}

VOID CMaterial::SetAlphaRef(DWORD refval)
{
    mMaterialData.AlphaRef = refval;
}
