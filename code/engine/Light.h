#pragma once

#include "RenderData.h"
#include "ReferenceManager.h"
#include "NodeComponent.h"

class ENGINE_API CLight : public CReferenceCounter, public CNodeComponent
{
public:
    CLight(unsigned int slot = 0);
    void Release();

    void Enable(bool state);
    void SetSlot(unsigned int slot);
    auto Clone() -> CLight*;

    void SetType(unsigned int type);
    void SetAmbient(D3DCOLORVALUE color);
    void SetDiffuse(D3DCOLORVALUE color);
    void SetSpecular(D3DCOLORVALUE color);
    void SetDirection(D3DXVECTOR3 dir);
    void SetPosition(D3DXVECTOR3 pos);
    void SetAttenuation(float constant, float linear, float quadratic);
    void SetFalloff(float falloff);
    void SetRange(float range);
    void SetInnerAngle(float theta);
    void SetOuterAngle(float phi);

    auto GetLightData() const -> D3DLIGHT9 { return mLightData; }
    auto GetSlot() const -> unsigned int { return mSlot; }

private:
    unsigned int mSlot;
    D3DLIGHT9 mLightData;

    void SetDefaults();
};
