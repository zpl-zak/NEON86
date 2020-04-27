#pragma once

#include "Effect.h"
#include "RenderData.h"

class CEffect
{
public:
    CEffect(LPCSTR effectPath);
    void Release();

    UINT Begin(LPCSTR technique);
    HRESULT End();

    UINT FindPass(LPCSTR passName);
    HRESULT BeginPass(UINT passID);
    HRESULT EndPass();
    HRESULT CommitChanges();

    // Uniforms
    void SetFloat(LPCSTR name, FLOAT value);
    void SetMatrix(LPCSTR name, D3DXMATRIX value, BOOL transpose=FALSE);
    void SetColor(LPCSTR name, D3DCOLORVALUE value);
    void SetTexture(LPCSTR name, IDirect3DTexture9* value);
    void SetVector3(LPCSTR name, D3DXVECTOR3 value);
    void SetVector4(LPCSTR name, D3DXVECTOR4 value);
    void SetBool(LPCSTR name, BOOL value);
private:
    ID3DXEffect* mEffect;

    void SetDefaults();
};