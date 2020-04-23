#include "stdafx.h"

#include "FaceGroup.h"
#include "NeonEngine.h"

#include "Material.h"
#include "Frustum.h"

CFaceGroup::CFaceGroup(void)
{
	ZeroMemory(&mData, sizeof(RENDERDATA));
	mVertCapacity = mIndexCapacity = 4;
	mVerts = (VERTEX*)malloc(mVertCapacity * sizeof(VERTEX));
	mIndices = (SHORT*)malloc(mIndexCapacity * sizeof(SHORT));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
}

VOID CFaceGroup::Release(void)
{
	SAFE_RELEASE(mData.mesh);
}

VOID CFaceGroup::SetMaterial(DWORD stage, CMaterial* tex)
{
	mData.stage = stage;
	mData.mat = tex;
}

VOID CFaceGroup::AddVertex(const VERTEX& vertex)
{
	mIsDirty = TRUE;
    if (mData.vertCount >= mVertCapacity)
    {
        mVertCapacity += 4;

        mVerts = (VERTEX*)realloc(mVerts, mVertCapacity * sizeof(VERTEX));
        
        if (!mVerts)
        {
            MessageBoxA(NULL, "Can't add vertex to mesh!", "Out of memory error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
    }

	mVerts[mData.vertCount++] = vertex;
}

VOID CFaceGroup::AddIndex(SHORT index)
{
	mIsDirty = TRUE;
    if (mData.indexCount >= mIndexCapacity)
    {
        mIndexCapacity += 4;

        mIndices = (SHORT*)realloc(mIndices, mIndexCapacity * sizeof(SHORT));

        if (!mVerts)
        {
            MessageBoxA(NULL, "Can't add index to mesh!", "Out of memory error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
    }

	mIndices[mData.indexCount++] = index;
}

VOID CFaceGroup::Draw(D3DXMATRIX* mat)
{
	if (!mData.mesh || mIsDirty)
		Build();

	if (mData.mat)
		mData.mat->Bind(mData.stage);

	if (mat)
	{
		mData.usesMatrix = TRUE;
		mData.matrix = *mat;
	}
	else mData.usesMatrix = FALSE;

	RENDERER->DrawMesh(mData);

	if (mData.mat)
		mData.mat->Unbind(mData.stage);
}

VOID CFaceGroup::CalculateNormals()
{
    if (!mData.mesh)
    {
        MessageBoxA(NULL, "Mesh is not built yet, you can not calculate normals!", "Mesh error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

	D3DXComputeNormals(mData.mesh, NULL);
}

VOID CFaceGroup::Build(void)
{
	LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
	VOID *vidMem = NULL;
	Release();

	if (mData.vertCount == 0)
		return;

	mData.primCount = ((mData.indexCount > 0) ? mData.indexCount : mData.vertCount)/3;
	
	D3DXCreateMesh(mData.primCount,
		mData.vertCount,
		D3DXMESH_MANAGED,
		meshVertexFormat,
		dev,
		&mData.mesh);
	
    mData.mesh->LockVertexBuffer(0, (VOID**)&vidMem);
    memcpy(vidMem, mVerts, mData.vertCount * sizeof(VERTEX));

    D3DXComputeBoundingSphere((D3DXVECTOR3*)mVerts,
        mData.vertCount,
        sizeof(VERTEX),
        &mData.meshOrigin,
        &mData.meshRadius);

	mData.mesh->UnlockVertexBuffer();

    if (mData.indexCount > 0)
    {
        mData.mesh->LockIndexBuffer(0, (VOID**)&vidMem);
        memcpy(vidMem, mIndices, mData.indexCount * sizeof(SHORT));
		mData.mesh->UnlockIndexBuffer();
    }

	mIsDirty = FALSE;
}

VOID CFaceGroup::Clear(void)
{
	Release();
	mData.vertCount = mData.primCount = mData.indexCount = 0;
	ZeroMemory(&mData, sizeof(RENDERDATA));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
    mVertCapacity = mIndexCapacity = 4;
    SAFE_FREE(mVerts);
    SAFE_FREE(mIndices);
    mVerts = (VERTEX*)malloc(mVertCapacity * sizeof(VERTEX));
    mIndices = (SHORT*)malloc(mIndexCapacity * sizeof(SHORT));
}

D3DVERTEXELEMENT9 meshVertexFormat[] =
{
    {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,0},
    {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,0},
    {0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
    {0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
    {0, 48, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    {0, 52, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0},
    D3DDECL_END()
};
