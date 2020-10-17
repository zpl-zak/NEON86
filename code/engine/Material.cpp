#include "stdafx.h"
#include "Material.h"

#include "Engine.h"
#include "VM.h"
#include "Renderer.h"
#include "FileSystem.h"

CMaterial::CMaterial(unsigned int slot, LPSTR texName): CMaterial()
{
    CreateTextureForSlot(slot, texName);
}

CMaterial::CMaterial(unsigned int slot, unsigned int w, unsigned int h): CMaterial()
{
    CreateTextureForSlot(slot, nullptr, w, h);
}

CMaterial::CMaterial()
{
    ZeroMemory(mTextureHandle, sizeof(mTextureHandle));
    ZeroMemory(mStats, sizeof(mStats));
    DefaultMaterial();
}

CMaterial::~CMaterial()
{
    Release();
}

CMaterial::CMaterial(unsigned int slot, LPVOID data, unsigned int size): CMaterial()
{
    CreateEmbeddedTextureForSlot(slot, data, size);
}

void CMaterial::DefaultMaterial()
{
    ZeroMemory(&mMaterialData, sizeof(mMaterialData));

    mMaterialData.Ambient.r = mMaterialData.Ambient.g = mMaterialData.Ambient.b = mMaterialData.Ambient.a = 1.0F;
    mMaterialData.Specular.r = mMaterialData.Specular.g = mMaterialData.Specular.b = mMaterialData.Specular.a = 1.0F;
    mMaterialData.Diffuse.r = mMaterialData.Diffuse.g = mMaterialData.Diffuse.b = mMaterialData.Diffuse.a = 1.0F;
    mMaterialData.Opacity = 1.0F;
    mMaterialData.Power = 1.0F;
    mMaterialData.Shaded = TRUE;
    mMaterialData.AlphaRef = 127;

    SetSamplerState(D3DSAMP_MINFILTER, TEXF_ANISOTROPIC);
    SetSamplerState(D3DSAMP_MIPFILTER, TEXF_ANISOTROPIC);
    SetSamplerState(D3DSAMP_MAGFILTER, TEXF_ANISOTROPIC);
    SetSamplerState(D3DSAMP_MAXANISOTROPY, 16);
}

void CMaterial::Release()
{
    if (DelRef())
    {
        for (unsigned int i = 0; i < MAX_TEXTURE_SLOTS; i++)
        {
            SAFE_RELEASE(mTextureHandle[i]);
        }

        delete this;
    }
}

void CMaterial::CreateTextureForSlot(unsigned int slot, LPSTR texName, unsigned int w, unsigned int h)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();

    if (texName == nullptr)
    {
        D3DXCreateTexture(dev, w, h, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &mTextureHandle[slot]);
    }
    else
    {
        FDATA img = FILESYSTEM->GetResource(texName);
        if (img.data == nullptr)
        {
            VM->PostError(CString::Format("Image not found: %s", texName));
            return;
        }
        D3DXCreateTextureFromFileInMemory(dev, img.data, img.size, &mTextureHandle[slot]);
    }
}

void CMaterial::CreateEmbeddedTextureForSlot(unsigned int slot, LPVOID data, unsigned int size)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();

    if (data == nullptr)
    {
        VM->PostError(CString("Embedded image is empty!"));
        return;
    }

    D3DXCreateTextureFromFileInMemory(dev, data, size, &mTextureHandle[slot]);
}

void CMaterial::SetUserTexture(unsigned int userSlot, LPDIRECT3DTEXTURE9 handle)
{
    if (userSlot >= (MAX_TEXTURE_SLOTS))
    {
        VM->PostError("User slot is invalid!");
        return;
    }

    mTextureHandle[userSlot] = handle;
}

void CMaterial::Bind(DWORD stage)
{
    RENDERER->SetMaterial(stage, this);

    /* Exit early if this is a NULL material. */
    /* Exit if we use shaders as well. Shaders use their own sampler properties. */
    if (RENDERER->GetActiveEffect())
    {
        return;
    }

    for (unsigned int i = 0; i < MAX_SAMPLER_STATES; i++)
    {
        RENDERER->SetSamplerState(stage, static_cast<D3DSAMPLERSTATETYPE>(i), mStats[i]);
    }
}

void CMaterial::Unbind(DWORD stage)
{
    RENDERER->SetMaterial(stage, nullptr);
}

auto CMaterial::Lock(unsigned int slot) -> LPVOID
{
    D3DLOCKED_RECT r;
    mTextureHandle[slot]->LockRect(0, &r, nullptr, 0);
    return r.pBits;
}

void CMaterial::UploadARGB(unsigned int slot, void* data, unsigned int size)
{
    D3DLOCKED_RECT r;
    mTextureHandle[slot]->LockRect(0, &r, nullptr, D3DLOCK_DISCARD);
    memcpy(r.pBits, data, size);
    mTextureHandle[slot]->UnlockRect(0);
}

void CMaterial::Unlock(unsigned int slot)
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

void CMaterial::SetPower(float val)
{
    mMaterialData.Power = val;
}

void CMaterial::SetOpacity(float val)
{
    mMaterialData.Opacity = val;
}

void CMaterial::SetShaded(bool state)
{
    mMaterialData.Shaded = state;
}

void CMaterial::SetAlphaIsTransparency(bool state)
{
    mMaterialData.AlphaIsTransparency = state;
}

void CMaterial::SetEnableAlphaTest(bool state)
{
    mMaterialData.AlphaTestEnabled = TRUE;
}

void CMaterial::SetAlphaRef(DWORD refval)
{
    mMaterialData.AlphaRef = refval;
}
