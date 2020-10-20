#include "StdAfx.h"
#include "Effect.h"
#include "Light.h"

#include "engine.h"
#include "VM.h"
#include "FileSystem.h"
#include "Renderer.h"

#include "BuiltinShaders.h"
#include <cstdio>

class CD3DIncludeImpl : ID3DXInclude
{
public:
    CD3DIncludeImpl();
    HRESULT _stdcall Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData,
                          unsigned int* pBytes) override;
    HRESULT _stdcall Close(LPCVOID pData) override;

private:
    bool mIsSystemInclude;
};

CD3DIncludeImpl::CD3DIncludeImpl()
{
    mIsSystemInclude = FALSE;
}

auto CD3DIncludeImpl::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData,
                           unsigned int* pBytes) -> HRESULT
{
    if (IncludeType == D3DXINC_SYSTEM)
    {
        mIsSystemInclude = TRUE;

        if ((strcmp(pFileName, "neon") == 0) || (strcmp(pFileName, "common.fx") == 0))
        {
            *ppData = _shader_common;
            *pBytes = static_cast<unsigned int>(strlen(_shader_common));
            return S_OK;
        }

        return E_FAIL;
    }

    FDATA f = FILESYSTEM->GetResource(pFileName);

    if (f.data == nullptr)
    {
        return E_FAIL;
    }

    *ppData = f.data;
    *pBytes = f.size;
    mIsSystemInclude = FALSE;
    return S_OK;
}

auto CD3DIncludeImpl::Close(LPCVOID pData) -> HRESULT
{
    if (mIsSystemInclude)
    {
        return S_OK;
    }

    FILESYSTEM->FreeResource(const_cast<LPVOID>(pData));
    return S_OK;
}

CEffect::CEffect()
{
    mEffect = nullptr;
}

void CEffect::LoadEffect(LPCSTR effectPath, bool debugMode)
{
    FDATA f = FILESYSTEM->GetResource(effectPath);

    if (f.data == nullptr)
    {
        VM->PostError(CString::Format("No effect found: %s", effectPath));
        return;
    }

    DWORD shaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_NO_PRESHADER;

    if (debugMode)
    {
        shaderFlags |= D3DXSHADER_DEBUG;
    }

    CD3DIncludeImpl inclHandler;

    LPD3DXBUFFER errors = nullptr;
    HRESULT hr = D3DXCreateEffect(
        RENDERER->GetDevice(),
        static_cast<LPCSTR>(f.data),
        static_cast<unsigned int>(f.size),
        nullptr,
        reinterpret_cast<LPD3DXINCLUDE>(&inclHandler),
        shaderFlags,
        nullptr,
        &mEffect,
        &errors);

    if (FAILED(hr))
    {
        if (errors != nullptr)
        {
            VM->PostError(static_cast<LPCSTR>(errors->GetBufferPointer()));
            errors->Release();
        }

        return;
    }

    if (errors != nullptr)
    {
        errors->Release();
    }
}

void CEffect::Release()
{
    if (DelRef())
        SAFE_RELEASE(mEffect);
}

auto CEffect::Begin(LPCSTR technique) -> unsigned int
{
    if (mEffect == nullptr)
    {
        return 0;
    }

    unsigned int numPasses = 0;
    const D3DXHANDLE techniqueID = mEffect->GetTechniqueByName(technique);

    if (FAILED(mEffect->SetTechnique(techniqueID)))
    {
        return 0;
    }

    HRESULT ok = mEffect->Begin(&numPasses, 0);

    if (FAILED(ok))
    {
        return 0;
    }

    RENDERER->SetActiveEffect(this);
    return numPasses;
}

auto CEffect::End() const -> HRESULT
{
    if (mEffect == nullptr)
    {
        return -1;
    }

    RENDERER->SetActiveEffect(nullptr);
    return mEffect->End();
}

auto CEffect::FindPass(LPCSTR passName) const -> unsigned int
{
    if (mEffect == nullptr)
    {
        return 0;
    }

    D3DXHANDLE curTech = mEffect->GetCurrentTechnique();
    D3DXTECHNIQUE_DESC td;
    mEffect->GetTechniqueDesc(curTech, &td);

    for (unsigned int i = 0; i < td.Passes; i++)
    {
        D3DXHANDLE h = mEffect->GetPass(curTech, i);
        D3DXPASS_DESC pd;
        mEffect->GetPassDesc(h, &pd);

        if (strcmp(passName, pd.Name) == 0)
        {
            return i;
        }
    }

    return static_cast<unsigned int>(-1);
}

auto CEffect::BeginPass(unsigned int passID) const -> HRESULT
{
    if (mEffect == nullptr)
    {
        return -1;
    }

    HRESULT ok = mEffect->BeginPass(passID);
    SetDefaults();

    return ok;
}

auto CEffect::EndPass() const -> HRESULT
{
    if (mEffect == nullptr)
    {
        return -1;
    }

    return mEffect->EndPass();
}

void CEffect::CommitChanges() const
{
    if (mEffect == nullptr)
    {
        return;
    }

    const auto res = mEffect->CommitChanges();

    if (FAILED(res))
    {
        VM->PostError(CString::Format("CEffect::CommitChanges error: %d\n", res));
    }
}

void CEffect::SetInteger(LPCSTR name, DWORD value) const
{
    if (mEffect == nullptr)
    {
        return;
    }

    mEffect->SetInt(name, value);
}

void CEffect::SetFloat(LPCSTR name, float value) const
{
    if (mEffect == nullptr)
    {
        return;
    }

    mEffect->SetFloat(name, value);
}

void CEffect::SetMatrix(LPCSTR name, const D3DXMATRIX& value, bool transpose) const
{
    if (mEffect == nullptr)
    {
        return;
    }

    if (transpose)
    {
        mEffect->SetMatrixTranspose(name, &value);
    }
    else
    {
        mEffect->SetMatrix(name, &value);
    }
}

void CEffect::SetColor(LPCSTR name, D3DCOLORVALUE value) const
{
    if (mEffect == nullptr)
    {
        return;
    }

    mEffect->SetValue(name, &value, sizeof(value));
}

void CEffect::SetTexture(LPCSTR name, IDirect3DTexture9* value) const
{
    if (mEffect == nullptr)
    {
        return;
    }

    mEffect->SetTexture(name, value);
}

void CEffect::SetLight(LPCSTR name, CLight* value) const
{
    if (mEffect == nullptr)
    {
        return;
    }

    if (value != nullptr)
    {
        SetBool(GetUniformName(name, "IsEnabled").Str(), TRUE);
        SetInteger(GetUniformName(name, "Type").Str(), value->GetLightData().Type);
        SetVector3(GetUniformName(name, "Position").Str(), value->GetLightData().Position);
        SetVector3(GetUniformName(name, "Direction").Str(), value->GetLightData().Direction);
        SetColor(GetUniformName(name, "Diffuse").Str(), value->GetLightData().Diffuse);
        SetColor(GetUniformName(name, "Ambient").Str(), value->GetLightData().Ambient);
        SetColor(GetUniformName(name, "Specular").Str(), value->GetLightData().Specular);
        SetFloat(GetUniformName(name, "Falloff").Str(), value->GetLightData().Falloff);
        SetFloat(GetUniformName(name, "Range").Str(), value->GetLightData().Range);
        SetFloat(GetUniformName(name, "ConstantAtten").Str(), value->GetLightData().Attenuation0);
        SetFloat(GetUniformName(name, "LinearAtten").Str(), value->GetLightData().Attenuation1);
        SetFloat(GetUniformName(name, "QuadraticAtten").Str(), value->GetLightData().Attenuation2);
    }
    else
    {
        SetBool(GetUniformName(name, "IsEnabled").Str(), FALSE);
    }
}

void CEffect::SetVector3(LPCSTR name, D3DXVECTOR3 value) const
{
    if (mEffect == nullptr)
    {
        return;
    }

    D3DXVECTOR4 vec4 = D3DXVECTOR4(value, 1.0F);
    mEffect->SetVector(name, &vec4);
}

void CEffect::SetVector4(LPCSTR name, D3DXVECTOR4 value) const
{
    if (mEffect == nullptr)
    {
        return;
    }

    mEffect->SetVector(name, &value);
}

void CEffect::SetBool(LPCSTR name, bool value) const
{
    if (mEffect == nullptr)
    {
        return;
    }

    mEffect->SetBool(name, static_cast<BOOL>(value));
}

void CEffect::SetDefaults() const
{
    if (mEffect == nullptr)
    {
        return;
    }

    const D3DXMATRIX p = RENDERER->GetDeviceMatrix(MATRIXKIND_PROJECTION);
    const D3DXMATRIX v = RENDERER->GetDeviceMatrix(MATRIXKIND_VIEW);
    D3DXMATRIX w;
    D3DXMatrixIdentity(&w);

    const D3DXMATRIX mvp = w * v * p;
    SetMatrix("NEON.Proj", p);
    SetMatrix("NEON.View", v);
    SetMatrix("NEON.World", w);
    SetMatrix("NEON.MVP", mvp);

    RENDERER->SetDefaultRenderStates();
}

auto CEffect::GetUniformName(LPCSTR base, LPCSTR field) -> CString
{
    return CString::Format("%s.%s", base, field);
}
