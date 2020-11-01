#include "StdAfx.h"

#include "Light.h"

#include "Engine.h"
#include "Renderer.h"

CLight::CLight(unsigned int slot)
{
    mSlot = slot;
    SetDefaults();
}

void CLight::Release()
{
    if (DelRef())
    {
        SetDefaults();
        delete this;
    }
}

void CLight::Enable(bool state)
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();

    dev->SetLight(mSlot, &mLightData);
    dev->LightEnable(mSlot, static_cast<BOOL>(state));
}

void CLight::SetSlot(unsigned int slot)
{
    mSlot = slot;
}

auto CLight::Clone() -> CLight*
{
    auto* lit = new CLight();

    lit->mSlot = mSlot;
    lit->mLightData = mLightData;

    return lit;
}

void CLight::SetType(unsigned int type)
{
    mLightData.Type = static_cast<D3DLIGHTTYPE>(type);
}

void CLight::SetAmbient(D3DCOLORVALUE color)
{
    memcpy(&mLightData.Ambient, &color, sizeof(D3DCOLORVALUE));
}

void CLight::SetDiffuse(D3DCOLORVALUE color)
{
    memcpy(&mLightData.Diffuse, &color, sizeof(D3DCOLORVALUE));
}

void CLight::SetSpecular(D3DCOLORVALUE color)
{
    memcpy(&mLightData.Specular, &color, sizeof(D3DCOLORVALUE));
}

void CLight::SetDirection(D3DXVECTOR3 dir)
{
    mLightData.Direction = dir;
}

void CLight::SetPosition(D3DXVECTOR3 pos)
{
    mLightData.Position = pos;
}

void CLight::SetAttenuation(float constant, float linear, float quadratic)
{
    mLightData.Attenuation0 = constant;
    mLightData.Attenuation1 = linear;
    mLightData.Attenuation2 = quadratic;
}

void CLight::SetFalloff(float falloff)
{
    mLightData.Falloff = falloff;
}

void CLight::SetRange(float range)
{
    mLightData.Range = range;
}

void CLight::SetInnerAngle(float theta)
{
    mLightData.Theta = theta;
}

void CLight::SetOuterAngle(float phi)
{
    mLightData.Phi = phi;
}

void CLight::SetDefaults()
{
    ZeroMemory(&mLightData, sizeof(mLightData));

    mLightData.Type = D3DLIGHT_DIRECTIONAL;

    mLightData.Diffuse = D3DXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
    mLightData.Ambient = D3DXCOLOR(0.0F, 0.0F, 0.0F, 1.0F);
    mLightData.Direction = D3DXVECTOR3(-1.0F, -0.3F, -1.0F);
    mLightData.Specular = D3DXCOLOR(0.0F, 0.0F, 0.0F, 1.0F);

    mLightData.Range = 0;
    mLightData.Falloff = 0;
    mLightData.Attenuation0 = 1;
    mLightData.Attenuation1 = 0;
    mLightData.Attenuation2 = 0;
}
