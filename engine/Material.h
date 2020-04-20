#pragma once

#include "system.h"
#include "StdAfx.h"

#include "RenderData.h"
#include "ReferenceManager.h"

class CMaterial: public CReference<CMaterial>
{
public:
    CMaterial(LPSTR texName = NULL, UINT w=1, UINT h=1);
    CMaterial(VOID* data, UINT size = 1);

    VOID DefaultMaterial();
    VOID Release(void);

    inline VOID SetSamplerState(UINT state, UINT value) { mStats[state] = value; }
    inline UINT GetSamplerState(UINT state) const { return mStats[state]; }
    inline LPDIRECT3DTEXTURE9 GetTextureHandle() { return mTextureHandle; }

    VOID Bind(DWORD stage);
    VOID Unbind(DWORD stage);
    VOID* Lock();
    VOID UploadRGB888(VOID* data, UINT size);
    VOID Unlock();

    VOID SetAmbient(D3DCOLORVALUE color);
    VOID SetDiffuse(D3DCOLORVALUE color);
    VOID SetSpecular(D3DCOLORVALUE color);
    VOID SetEmission(D3DCOLORVALUE color);
    VOID SetPower(FLOAT val);

private:
    LPDIRECT3DTEXTURE9 mTextureHandle;
    D3DMATERIAL9 mMaterialData;

    UINT mStats[MAX_SAMPLER_STATES];
};

