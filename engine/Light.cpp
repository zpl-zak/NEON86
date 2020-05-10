#include "StdAfx.h"

#include "Light.h"
#include "NeonEngine.h"

CLight::CLight(UINT slot)
{
    mSlot = slot;
    SetDefaults();
}

VOID CLight::Release()
{
    SetDefaults();
    Enable(FALSE);
}

VOID CLight::Enable(BOOL state)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();

    dev->SetLight(mSlot, &mLightData);
    dev->LightEnable(mSlot, state);
}

VOID CLight::SetSlot(UINT slot)
{
    mSlot = slot;
}

VOID CLight::SetType(UINT type)
{
    mLightData.Type = (D3DLIGHTTYPE)type;
}

VOID CLight::SetAmbient(D3DCOLORVALUE color)
{
    memcpy(&mLightData.Ambient, &color, sizeof(D3DCOLORVALUE));
}

VOID CLight::SetDiffuse(D3DCOLORVALUE color)
{
    memcpy(&mLightData.Diffuse, &color, sizeof(D3DCOLORVALUE));
}

VOID CLight::SetSpecular(D3DCOLORVALUE color)
{
    memcpy(&mLightData.Specular, &color, sizeof(D3DCOLORVALUE));
}

VOID CLight::SetDirection(D3DXVECTOR3 dir)
{
    mLightData.Direction = dir;
}

VOID CLight::SetPosition(D3DXVECTOR3 pos)
{
    mLightData.Position = pos;
}

VOID CLight::SetAttenuation(FLOAT constant, FLOAT linear, FLOAT quadratic)
{
    mLightData.Attenuation0 = constant;
    mLightData.Attenuation1 = linear;
    mLightData.Attenuation2 = quadratic;
}

VOID CLight::SetFalloff(FLOAT falloff)
{
    mLightData.Falloff = falloff;
}

VOID CLight::SetRange(FLOAT range)
{
    mLightData.Range = range;
}

VOID CLight::SetInnerAngle(FLOAT theta)
{
    mLightData.Theta = theta;
}

VOID CLight::SetOuterAngle(FLOAT phi)
{
    mLightData.Phi = phi;
}

VOID CLight::SetDefaults()
{
    ZeroMemory(&mLightData, sizeof(mLightData));

    mLightData.Type = D3DLIGHT_DIRECTIONAL;

    mLightData.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    mLightData.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
    mLightData.Direction = D3DXVECTOR3(-1.0f, -0.3f, -1.0f);
    mLightData.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

    mLightData.Range = 0;
    mLightData.Falloff = 0;
    mLightData.Attenuation0 = 1;
    mLightData.Attenuation1 = 0;
    mLightData.Attenuation2 = 0;
}
