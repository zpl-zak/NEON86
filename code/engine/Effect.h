#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class CLight;

class ENGINE_API CEffect : public CAllocable<CEffect>
{
public:
    CEffect();
    void LoadEffect(LPCSTR effectPath);
    void Release();

    auto Begin(LPCSTR technique) -> unsigned int;
    auto End() -> HRESULT;

    auto FindPass(LPCSTR passName) -> unsigned int;
    auto BeginPass(unsigned int passID) -> HRESULT;
    auto EndPass() -> HRESULT;
    auto CommitChanges() -> HRESULT;

    // Uniforms
    void SetInteger(LPCSTR name, DWORD value);
    void SetFloat(LPCSTR name, float value);
    void SetMatrix(LPCSTR name, D3DXMATRIX value, bool transpose = FALSE);
    void SetColor(LPCSTR name, D3DCOLORVALUE value);
    void SetTexture(LPCSTR name, IDirect3DTexture9* value);
    void SetLight(LPCSTR name, CLight* value);
    void SetVector3(LPCSTR name, D3DXVECTOR3 value);
    void SetVector4(LPCSTR name, D3DXVECTOR4 value);
    void SetBool(LPCSTR name, bool value);
private:
    ID3DXEffect* mEffect;

    void SetDefaults();
    auto GetUniformName(LPCSTR base, LPCSTR field) -> LPCSTR;
};
