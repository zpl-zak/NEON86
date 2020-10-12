#pragma once

#include "system.h"
#include "StdAfx.h"

#include "RenderData.h"
#include "ReferenceManager.h"

class ENGINE_API CMaterial : public CReferenceCounter, CAllocable<CMaterial>, NoCopyAssign
{
public:
    CMaterial(unsigned int slot, LPSTR texName);
    CMaterial(unsigned int slot, unsigned int w, unsigned int h);
    CMaterial(unsigned int slot, LPVOID data, unsigned int size = 1);
    CMaterial();
    ~CMaterial();

    void DefaultMaterial(void);
    void Release(void);

    void CreateTextureForSlot(unsigned int slot, LPSTR texName = nullptr, unsigned int w = 1, unsigned int h = 1);
    void CreateEmbeddedTextureForSlot(unsigned int slot, LPVOID data, unsigned int size);

    void SetSamplerState(unsigned int state, unsigned int value) { mStats[state] = value; }
    auto GetSamplerState(unsigned int state) const -> unsigned int { return mStats[state]; }
    auto GetTextureHandle(unsigned int slot = TEXTURESLOT_ALBEDO) -> LPDIRECT3DTEXTURE9 { return mTextureHandle[slot]; }

    void SetUserTexture(unsigned int userSlot, LPDIRECT3DTEXTURE9 handle);
    void Bind(DWORD stage);
    static void Unbind(DWORD stage);
    auto Lock(unsigned int slot = TEXTURESLOT_ALBEDO) -> LPVOID;
    void UploadARGB(unsigned int slot, void* data, unsigned int size);
    void Unlock(unsigned int slot = TEXTURESLOT_ALBEDO);

    void SetAmbient(D3DCOLORVALUE color);
    void SetDiffuse(D3DCOLORVALUE color);
    void SetSpecular(D3DCOLORVALUE color);
    void SetEmission(D3DCOLORVALUE color);
    void SetPower(float val);
    void SetOpacity(float val);
    void SetShaded(bool state);
    void SetAlphaIsTransparency(bool state);
    void SetEnableAlphaTest(bool state);
    void SetAlphaRef(DWORD refval);

    auto GetMaterialData() const -> MATERIAL { return mMaterialData; }
    auto IsTransparent() const -> bool { return mMaterialData.Opacity < 1.0f || mMaterialData.AlphaIsTransparency; }

private:
    LPDIRECT3DTEXTURE9 mTextureHandle[MAX_TEXTURE_SLOTS];
    MATERIAL mMaterialData;

    unsigned int mStats[MAX_SAMPLER_STATES];
};
