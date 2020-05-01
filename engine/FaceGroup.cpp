#include "stdafx.h"

#include "FaceGroup.h"
#include "NeonEngine.h"

#include "Material.h"
#include "Frustum.h"

CFaceGroup::CFaceGroup(VOID)
{
	ZeroMemory(&mData, sizeof(RENDERDATA));
	mVertCapacity = mIndexCapacity = 4;
	mVerts = (VERTEX*)neon_malloc(mVertCapacity * sizeof(VERTEX));
	mIndices = (SHORT*)neon_malloc(mIndexCapacity * sizeof(SHORT));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
}

VOID CFaceGroup::Release(VOID)
{
	SAFE_FREE(mVerts);
	SAFE_FREE(mIndices);
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

        mVerts = (VERTEX*)neon_realloc(mVerts, mVertCapacity * sizeof(VERTEX));
        
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

        mIndices = (SHORT*)neon_realloc(mIndices, mIndexCapacity * sizeof(SHORT));

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

	mData.mat->Bind(mData.stage);

	if (mat)
	{
		mData.usesMatrix = TRUE;
		mData.matrix = *mat;
	}
	else mData.usesMatrix = FALSE;

	RENDERER->DrawMesh(mData);

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

VOID CFaceGroup::Build(VOID)
{
	LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
	VOID *vidMem = NULL;
	SAFE_RELEASE(mData.mesh);

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

VOID CFaceGroup::Clear(VOID)
{
	Release();
	mData.vertCount = mData.primCount = mData.indexCount = 0;
	ZeroMemory(&mData, sizeof(RENDERDATA));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
    mVertCapacity = mIndexCapacity = 4;
    SAFE_FREE(mVerts);
    SAFE_FREE(mIndices);
    mVerts = (VERTEX*)neon_malloc(mVertCapacity * sizeof(VERTEX));
    mIndices = (SHORT*)neon_malloc(mIndexCapacity * sizeof(SHORT));
}
