#include "StdAfx.h"
#include "Effect.h"
#include "Light.h"

#include "engine.h"
#include "FileSystem.h"
#include "Renderer.h"

#include "BuiltinShaders.h"
#include <cstdio>

class CD3DIncludeImpl: ID3DXInclude
{
public:
    CD3DIncludeImpl();
    HRESULT _stdcall Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;
    HRESULT _stdcall Close(LPCVOID pData) override;

private:
    BOOL mIsSystemInclude;
};

CD3DIncludeImpl::CD3DIncludeImpl()
{
    mIsSystemInclude = FALSE;
}

HRESULT CD3DIncludeImpl::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
    if (IncludeType == D3DXINC_SYSTEM)
    {
        mIsSystemInclude = TRUE;

        if (!strcmp(pFileName, "neon") || !strcmp(pFileName, "common.fx"))
        {
            *ppData = _shader_common;
            *pBytes = strlen(_shader_common);
            return S_OK;
        }
        
        return E_FAIL;
    }

    FDATA f = FILESYSTEM->GetResource(RESOURCEKIND_USER, pFileName);

    if (!f.data)
        return E_FAIL;

    *ppData = f.data;
    *pBytes = f.size;
    mIsSystemInclude = FALSE;
    return S_OK;
}

HRESULT CD3DIncludeImpl::Close(LPCVOID pData)
{
    if (mIsSystemInclude)
        return S_OK;

    FILESYSTEM->FreeResource((LPVOID)pData);
    return S_OK;
}

CEffect::CEffect(LPCSTR effectPath): CAllocable()
{
    mEffect = NULL;

    FDATA f = FILESYSTEM->GetResource(RESOURCEKIND_USER, effectPath);

    if (!f.data)
    {
        MessageBoxA(NULL, "No effect found!", "Resource error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    DWORD shaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_NO_PRESHADER;
    //shaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT | D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT | D3DXSHADER_DEBUG;

    CD3DIncludeImpl inclHandler;

    LPD3DXBUFFER errors = NULL;
    HRESULT hr = D3DXCreateEffect(
        RENDERER->GetDevice(),
        (LPCSTR)f.data,
        (UINT)f.size,
        NULL,
        (LPD3DXINCLUDE)&inclHandler,
        shaderFlags,
        NULL,
        &mEffect,
        &errors);

    if (FAILED(hr))
    {
        MessageBoxA(NULL, "Effect has issues!", "Effect error", MB_OK);

        if (errors)
        {
            MessageBoxA(NULL, (LPCSTR)errors->GetBufferPointer(), "Effect error", MB_OK);
            errors->Release();
        }

        ENGINE->Shutdown();
        return;
    }

    if (errors)
        errors->Release();
}

VOID CEffect::Release()
{
    SAFE_RELEASE(mEffect);
}

UINT CEffect::Begin(LPCSTR technique)
{
    UINT numPasses=0;
    D3DXHANDLE techniqueID = mEffect->GetTechniqueByName(technique);

    if (FAILED(mEffect->SetTechnique(techniqueID)))
        return 0;

    HRESULT ok = mEffect->Begin(&numPasses, 0);

    if (FAILED(ok))
        return 0;

    RENDERER->SetActiveEffect(this);
    return numPasses;
}

HRESULT CEffect::End()
{
    RENDERER->SetActiveEffect(NULL);
    return mEffect->End();
}

UINT CEffect::FindPass(LPCSTR passName)
{
    D3DXHANDLE curTech = mEffect->GetCurrentTechnique();
    D3DXTECHNIQUE_DESC td;
    mEffect->GetTechniqueDesc(curTech, &td);

    for (UINT i=0; i<td.Passes;i++)
    {
        D3DXHANDLE h = mEffect->GetPass(curTech, i);
        D3DXPASS_DESC pd;
        mEffect->GetPassDesc(h, &pd);

        if (!strcmp(passName, pd.Name))
            return i;
    }

    return (UINT)-1;
}

HRESULT CEffect::BeginPass(UINT passID)
{
    HRESULT ok = mEffect->BeginPass(passID);
    SetDefaults();

    return ok;
}

HRESULT CEffect::EndPass()
{
    return mEffect->EndPass();
}

HRESULT CEffect::CommitChanges()
{
    return mEffect->CommitChanges();
}

VOID CEffect::SetInteger(LPCSTR name, DWORD value)
{
    mEffect->SetInt(name, value);
}

VOID CEffect::SetFloat(LPCSTR name, FLOAT value)
{
    mEffect->SetFloat(name, value);
}

VOID CEffect::SetMatrix(LPCSTR name, D3DXMATRIX value, BOOL transpose)
{
    if (transpose)
        mEffect->SetMatrixTranspose(name, &value);
    else
        mEffect->SetMatrix(name, &value);
}

VOID CEffect::SetColor(LPCSTR name, D3DCOLORVALUE value)
{
    mEffect->SetValue(name, &value, sizeof(value));
}

VOID CEffect::SetTexture(LPCSTR name, IDirect3DTexture9* value)
{
    mEffect->SetTexture(name, value);
}

VOID CEffect::SetLight(LPCSTR name, CLight* value)
{
    if (value) {
        SetBool(GetUniformName(name, "IsEnabled"), TRUE);
        SetInteger(GetUniformName(name, "Type"), value->GetLightData().Type);
        SetVector3(GetUniformName(name, "Position"), value->GetLightData().Position);
        SetVector3(GetUniformName(name, "Direction"), value->GetLightData().Direction);
        SetColor(GetUniformName(name, "Diffuse"), value->GetLightData().Diffuse);
        SetColor(GetUniformName(name, "Ambient"), value->GetLightData().Ambient);
        SetColor(GetUniformName(name, "Specular"), value->GetLightData().Specular);
        SetFloat(GetUniformName(name, "Falloff"), value->GetLightData().Falloff);
        SetFloat(GetUniformName(name, "Range"), value->GetLightData().Range);
        SetFloat(GetUniformName(name, "ConstantAtten"), value->GetLightData().Attenuation0);
        SetFloat(GetUniformName(name, "LinearAtten"), value->GetLightData().Attenuation1);
        SetFloat(GetUniformName(name, "QuadraticAtten"), value->GetLightData().Attenuation2);
    }
    else {
        SetBool(GetUniformName(name, "IsEnabled"), FALSE);
    }
    /*int Type;
    float3 Position;
    float3 Direction;
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;

    float Falloff;
    float Range;

    float ConstantAtten;
    float LinearAtten;
    float QuadraticAtten;*/
}

VOID CEffect::SetVector3(LPCSTR name, D3DXVECTOR3 value)
{
    D3DXVECTOR4 vec4 = D3DXVECTOR4(value, 1.0f);
    mEffect->SetVector(name, &vec4);
}

VOID CEffect::SetVector4(LPCSTR name, D3DXVECTOR4 value)
{
    mEffect->SetVector(name, &value);
}

VOID CEffect::SetBool(LPCSTR name, BOOL value)
{
    mEffect->SetBool(name, value);
}

VOID CEffect::SetDefaults()
{
    D3DXMATRIX p = RENDERER->GetDeviceMatrix(MATRIXKIND_PROJECTION);
    D3DXMATRIX v = RENDERER->GetDeviceMatrix(MATRIXKIND_VIEW);
    D3DXMATRIX w;
    D3DXMatrixIdentity(&w);

    D3DXMATRIX mvp = w * v * p;
    SetMatrix("NEON.Proj", p);
    SetMatrix("NEON.View", v);
    SetMatrix("NEON.World", w);
    SetMatrix("NEON.MVP", mvp);

    RENDERER->SetDefaultRenderStates();
}

LPCSTR CEffect::GetUniformName(LPCSTR base, LPCSTR field)
{
    static char buffer[512] = { 0 };

    sprintf_s(buffer, 512, "%s.%s", base, field);
    return (LPCSTR)buffer;
}
