#pragma once

#include "RenderData.h"
#include "ReferenceManager.h"
#include "NodeComponent.h"

class ENGINE_API CLight : public CReferenceCounter, public CNodeComponent
{
public:
    CLight(UINT slot = 0);
    VOID Release();

    VOID Enable(BOOL state);
    VOID SetSlot(UINT slot);
    auto Clone() -> CLight*;

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

    auto GetLightData() const -> D3DLIGHT9 { return mLightData; }
    auto GetSlot() const -> UINT { return mSlot; }

private:
    UINT mSlot;
    D3DLIGHT9 mLightData;

    VOID SetDefaults();
};
