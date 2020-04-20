#pragma once

#include "system.h"
#include "StdAfx.h"

#include "RenderData.h"
#include "ReferenceManager.h"

class CMaterial: public CReference<CMaterial>
{
public:
    CMaterial(LPSTR texName = NULL, UINT w=1, UINT h=1);
    CMaterial(void* data, UINT size = 1);

    void DefaultMaterial();
    void Release(void);

    inline void SetSamplerState(UINT state, UINT value) { mStats[state] = value; }
    inline UINT GetSamplerState(UINT state) const { return mStats[state]; }
    inline LPDIRECT3DTEXTURE9 GetTextureHandle() { return mTextureHandle; }

    void Bind(DWORD stage);
    void Unbind(DWORD stage);
    void* Lock();
    void UploadRGB888(VOID* data, UINT size);
    void Unlock();

    void SetAmbient(D3DCOLORVALUE color);
    void SetDiffuse(D3DCOLORVALUE color);
    void SetSpecular(D3DCOLORVALUE color);
    void SetEmission(D3DCOLORVALUE color);
    void SetPower(FLOAT val);
    void SetOpacity(FLOAT val);

private:
    LPDIRECT3DTEXTURE9 mTextureHandle;
    MATERIAL mMaterialData;

    UINT mStats[MAX_SAMPLER_STATES];
};

