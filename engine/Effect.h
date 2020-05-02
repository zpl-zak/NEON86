#pragma once

#include "Effect.h"
#include "RenderData.h"

class CEffect
{
public:
    CEffect(LPCSTR effectPath);
    VOID Release();

    UINT Begin(LPCSTR technique);
    HRESULT End();

    UINT FindPass(LPCSTR passName);
    HRESULT BeginPass(UINT passID);
    HRESULT EndPass();
    HRESULT CommitChanges();

    // Uniforms
    VOID SetFloat(LPCSTR name, FLOAT value);
    VOID SetMatrix(LPCSTR name, D3DXMATRIX value, BOOL transpose = FALSE);
    VOID SetColor(LPCSTR name, D3DCOLORVALUE value);
    VOID SetTexture(LPCSTR name, IDirect3DTexture9* value);
    VOID SetVector3(LPCSTR name, D3DXVECTOR3 value);
    VOID SetVector4(LPCSTR name, D3DXVECTOR4 value);
    VOID SetBool(LPCSTR name, BOOL value);
private:
    ID3DXEffect* mEffect;

    VOID SetDefaults();
};