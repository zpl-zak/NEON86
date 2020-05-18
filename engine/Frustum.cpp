#include "stdafx.h"
#include "Frustum.h"

#include "Engine.h"
#include "Renderer.h"

CFrustum::CFrustum()
{
    
}

VOID CFrustum::Release(VOID)
{

}

VOID CFrustum::Build(VOID)
{
    //FLOAT zmin, r;
    D3DXMATRIX mat;

    D3DXMATRIX projMat = RENDERER->GetDeviceMatrix(MATRIXKIND_PROJECTION);
    D3DXMATRIX viewMat = RENDERER->GetDeviceMatrix(MATRIXKIND_VIEW);
    
    /*zmin = -projMat._43 / projMat._33;
    r = 100.0f;

    projMat._33 = r;
    projMat._43 = -r * zmin;*/

    D3DXMatrixMultiply(&mat, &viewMat, &projMat);

    // near
    mPlanes[0].a = mat._14 + mat._13;
    mPlanes[0].b = mat._24 + mat._23;
    mPlanes[0].c = mat._34 + mat._33;
    mPlanes[0].d = mat._44 + mat._43;
    D3DXPlaneNormalize(&mPlanes[0], &mPlanes[0]);

    // far
    mPlanes[1].a = mat._14 - mat._13;
    mPlanes[1].b = mat._24 - mat._23;
    mPlanes[1].c = mat._34 - mat._33;
    mPlanes[1].d = mat._44 - mat._43;
    D3DXPlaneNormalize(&mPlanes[1], &mPlanes[1]);

    // left
    mPlanes[2].a = mat._14 + mat._11;
    mPlanes[2].b = mat._24 + mat._21;
    mPlanes[2].c = mat._34 + mat._31;
    mPlanes[2].d = mat._44 + mat._41;
    D3DXPlaneNormalize(&mPlanes[2], &mPlanes[2]);

    // right
    mPlanes[3].a = mat._14 - mat._11;
    mPlanes[3].b = mat._24 - mat._21;
    mPlanes[3].c = mat._34 - mat._31;
    mPlanes[3].d = mat._44 - mat._41;
    D3DXPlaneNormalize(&mPlanes[3], &mPlanes[3]);

    // top
    mPlanes[4].a = mat._14 + mat._12;
    mPlanes[4].b = mat._24 + mat._22;
    mPlanes[4].c = mat._34 + mat._32;
    mPlanes[4].d = mat._44 + mat._42;
    D3DXPlaneNormalize(&mPlanes[4], &mPlanes[4]);

    // bottom
    mPlanes[4].a = mat._14 - mat._12;
    mPlanes[4].b = mat._24 - mat._22;
    mPlanes[4].c = mat._34 - mat._32;
    mPlanes[4].d = mat._44 - mat._42;
    D3DXPlaneNormalize(&mPlanes[4], &mPlanes[4]);
}

BOOL CFrustum::IsPointVisible(D3DXVECTOR3 pos)
{
    for (UINT i = 0; i < 6; i++)
    {
        if (D3DXPlaneDotCoord(&mPlanes[i], &pos) < 0.0F)
            return FALSE;
    }
    
    return TRUE;
}

BOOL CFrustum::IsBoxVisible(D3DXVECTOR3 pos, RENDERBOUNDS bounds)
{
    return 0;
}

BOOL CFrustum::IsSphereVisible(D3DXVECTOR3 pos, FLOAT radius)
{
    for (UINT i = 0; i < 6; i++)
    {
        if (D3DXPlaneDotCoord(&mPlanes[i], &pos) < -radius)
            return FALSE;
    }

    return TRUE;
}
