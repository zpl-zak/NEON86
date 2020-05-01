#pragma once

#include "system.h"
#include "StdAfx.h"

#include "RenderData.h"

class CFrustum
{
public:
    CFrustum();
    VOID Release(VOID);

    VOID Build(VOID);
    
    BOOL IsPointVisible(D3DXVECTOR3 pos);
    BOOL IsBoxVisible(D3DXVECTOR3 pos, RENDERBOUNDS bounds);
    BOOL IsSphereVisible(D3DXVECTOR3 pos, FLOAT radius);
private:
    D3DXPLANE mPlanes[6];
};

