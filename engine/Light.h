#pragma once

#include "RenderData.h"

class CLight
{
public:
    CLight(UINT slot=0);
    VOID Release();

    VOID Enable(BOOL state);
    VOID SetSlot(UINT slot);

    VOID SetType(UINT type);
    VOID SetAmbient(D3DCOLORVALUE color);
    VOID SetDiffuse(D3DCOLORVALUE color);
    VOID SetSpecular(D3DCOLORVALUE color);
    VOID SetDirection(D3DXVECTOR3 dir);
    VOID SetPosition(D3DXVECTOR3 pos);
    VOID SetAttenuation(FLOAT constant, FLOAT linear, FLOAT quadratic);
    VOID SetFalloff(FLOAT falloff);
    VOID SetRange(FLOAT range);
    VOID SetInnerAngle(FLOAT theta);
    VOID SetOuterAngle(FLOAT phi);

    inline D3DLIGHT9 GetLightData() { return mLightData; }

private:
    UINT mSlot;
    D3DLIGHT9 mLightData;

    VOID SetDefaults();
};