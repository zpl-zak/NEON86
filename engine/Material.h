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
    CMaterial(UINT slot, VOID* data, UINT size = 1);
    CMaterial();

    VOID DefaultMaterial();
    VOID Release(VOID);

    VOID CreateTextureForSlot(UINT slot, LPSTR texName=NULL, UINT w=1, UINT h=1);
    VOID CreateEmbeddedTextureForSlot(UINT slot, VOID* data, UINT size);
    
    inline VOID SetSamplerState(UINT state, UINT value) { mStats[state] = value; }
    inline UINT GetSamplerState(UINT state) const { return mStats[state]; }
    inline LPDIRECT3DTEXTURE9 GetTextureHandle(UINT slot=TEXTURESLOT_ALBEDO) { return mTextureHandle[slot]; }
    inline LPDIRECT3DTEXTURE9 GetUserTextureHandle(UINT userSlot) { return mTextureHandle[userSlot]; }

    VOID SetUserTexture(UINT userSlot, LPDIRECT3DTEXTURE9 handle);
    VOID Bind(DWORD stage);
    VOID Unbind(DWORD stage);
    VOID* Lock(UINT slot=TEXTURESLOT_ALBEDO);
    VOID UploadARGB(UINT slot, VOID* data, UINT size);
    VOID Unlock(UINT slot = TEXTURESLOT_ALBEDO);

    VOID SetAmbient(D3DCOLORVALUE color);
    VOID SetDiffuse(D3DCOLORVALUE color);
    VOID SetSpecular(D3DCOLORVALUE color);
    VOID SetEmission(D3DCOLORVALUE color);
    VOID SetPower(FLOAT val);
    VOID SetOpacity(FLOAT val);

    inline MATERIAL GetMaterialData() const { return mMaterialData; }

private:
    LPDIRECT3DTEXTURE9 mTextureHandle[MAX_TEXTURE_SLOTS];
    MATERIAL mMaterialData;

    UINT mStats[MAX_SAMPLER_STATES];
};

