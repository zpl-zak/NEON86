#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class CLight;

class ENGINE_API CEffect : CReferenceCounter, public CAllocable<CEffect>
{
public:
    CEffect();
    void LoadEffect(LPCSTR effectPath, bool debugMode);
    void Release();

    auto Begin(LPCSTR technique) -> unsigned int;
    auto End() const -> HRESULT;

    auto FindPass(LPCSTR passName) const -> unsigned int;
    auto BeginPass(unsigned int passID) const -> HRESULT;
    auto EndPass() const -> HRESULT;
    void CommitChanges() const;

    // Uniforms
    void SetInteger(LPCSTR name, DWORD value) const;
    void SetFloat(LPCSTR name, float value) const;
    void SetMatrix(LPCSTR name, const D3DXMATRIX& value, bool transpose = FALSE) const;
    void SetColor(LPCSTR name, D3DCOLORVALUE value) const;
    void SetTexture(LPCSTR name, IDirect3DTexture9* value) const;
    void SetLight(LPCSTR name, CLight* value) const;
    void SetVector3(LPCSTR name, D3DXVECTOR3 value) const;
    void SetVector4(LPCSTR name, D3DXVECTOR4 value) const;
    void SetBool(LPCSTR name, bool value) const;
private:
    ID3DXEffect* mEffect;

    void SetDefaults() const;
    static auto GetUniformName(LPCSTR base, LPCSTR field) -> CString;
};
