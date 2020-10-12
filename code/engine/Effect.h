#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class CLight;

class ENGINE_API CEffect : public CAllocable<CEffect>
{
public:
    CEffect();
    VOID LoadEffect(LPCSTR effectPath);
    VOID Release();

    auto Begin(LPCSTR technique) -> UINT;
    auto End() -> HRESULT;

    auto FindPass(LPCSTR passName) -> UINT;
    auto BeginPass(UINT passID) -> HRESULT;
    auto EndPass() -> HRESULT;
    auto CommitChanges() -> HRESULT;

    // Uniforms
    VOID SetInteger(LPCSTR name, DWORD value);
    VOID SetFloat(LPCSTR name, FLOAT value);
    VOID SetMatrix(LPCSTR name, D3DXMATRIX value, BOOL transpose = FALSE);
    VOID SetColor(LPCSTR name, D3DCOLORVALUE value);
    VOID SetTexture(LPCSTR name, IDirect3DTexture9* value);
    VOID SetLight(LPCSTR name, CLight* value);
    VOID SetVector3(LPCSTR name, D3DXVECTOR3 value);
    VOID SetVector4(LPCSTR name, D3DXVECTOR4 value);
    VOID SetBool(LPCSTR name, BOOL value);
private:
    ID3DXEffect* mEffect;

    VOID SetDefaults();
    auto GetUniformName(LPCSTR base, LPCSTR field) -> LPCSTR;
};
