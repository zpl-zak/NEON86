#pragma once

#include "system.h"
#include "StdAfx.h"

#include "RenderData.h"

class CTexture
{
public:
    CTexture(LPSTR texName = NULL, UINT w=1, UINT h=1);
    VOID Release(void);

    inline VOID SetSamplerState(UINT state, UINT value) { mStats[state] = value; }
    inline UINT GetSamplerState(UINT state) const { return mStats[state]; }
    inline LPDIRECT3DTEXTURE9 GetTextureHandle() { return mTextureHandle; }

    VOID Bind(DWORD stage);
    VOID Unbind(DWORD stage);

private:
    LPDIRECT3DTEXTURE9 mTextureHandle;

    UINT mStats[MAX_SAMPLER_STATES];
};

