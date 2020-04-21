#pragma once

#include "system.h"
#include "StdAfx.h"

#include "RenderData.h"
#include "ReferenceManager.h"

class CMaterial: public CReference<CMaterial>
{
public:
    CMaterial(UINT slot, LPSTR texName);
    CMaterial(UINT slot, UINT w, UINT h);
    CMaterial(UINT slot, void* data, UINT size = 1);
    CMaterial();

    void DefaultMaterial();
    void Release(void);

    void CreateTextureForSlot(UINT slot, LPSTR texName=NULL, UINT w=1, UINT h=1);
    void CreateEmbeddedTextureForSlot(UINT slot, void* data, UINT size);
    inline void SetSamplerState(UINT state, UINT value) { mStats[state] = value; }
    inline UINT GetSamplerState(UINT state) const { return mStats[state]; }
    inline LPDIRECT3DTEXTURE9 GetTextureHandle(UINT slot=TEXTURESLOT_ALBEDO) { return mTextureHandle[slot]; }

    void Bind(DWORD stage);
    void Unbind(DWORD stage);
    void* Lock(UINT slot=TEXTURESLOT_ALBEDO);
    void UploadRGB888(UINT slot, VOID* data, UINT size);
    void Unlock(UINT slot = TEXTURESLOT_ALBEDO);

    void SetAmbient(D3DCOLORVALUE color);
    void SetDiffuse(D3DCOLORVALUE color);
    void SetSpecular(D3DCOLORVALUE color);
    void SetEmission(D3DCOLORVALUE color);
    void SetPower(FLOAT val);
    void SetOpacity(FLOAT val);

    inline MATERIAL GetMaterialData() const { return mMaterialData; }

private:
    LPDIRECT3DTEXTURE9 mTextureHandle[MAX_TEXTURE_SLOTS];
    MATERIAL mMaterialData;

    UINT mStats[MAX_SAMPLER_STATES];
};

