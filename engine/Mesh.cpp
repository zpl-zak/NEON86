#include "stdafx.h"

#include "Mesh.h"
#include "NeonEngine.h"

#include "Texture.h"
#include "Frustum.h"

CMesh::CMesh(void)
{
	ZeroMemory(&mData, sizeof(RENDERDATA));
	ZeroMemory(mVerts, sizeof(mVerts));
	ZeroMemory(mIndices, sizeof(mIndices));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
}

VOID CMesh::Release(void)
{
	SAFE_RELEASE(mData.mesh);
}

VOID CMesh::SetTexture(DWORD stage, CTexture* tex)
{
	mData.stage = stage;
	mData.tex = tex;
}

VOID CMesh::AddVertex(const VERTEX& vertex)
{
	mIsDirty = TRUE;
	mVerts[mData.vertCount++] = vertex;
}

VOID CMesh::AddIndex(SHORT index)
{
	mIsDirty = TRUE;
	mIndices[mData.indexCount++] = index;
}

VOID CMesh::Draw(D3DXMATRIX* mat)
{
	if (!mData.mesh || mIsDirty)
		Build();

	if (mData.tex)
		mData.tex->Bind(mData.stage);

	if (mat)
	{
		mData.usesMatrix = TRUE;
		mData.matrix = *mat;
	}
	else mData.usesMatrix = FALSE;

	RENDERER->PushPolygon(mData);

	if (mData.tex)
		mData.tex->Unbind(mData.stage);
}

VOID CMesh::Build(void)
{
	LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
	VOID *vidMem = NULL;
	Release();

	if (mData.vertCount == 0)
		return;

	mData.primCount = ((mData.indexCount > 0) ? mData.indexCount : mData.vertCount)/3;
	
	D3DXCreateMeshFVF(mData.primCount,
		mData.vertCount,
		D3DXMESH_MANAGED,
		NEONFVF,
		dev,
		&mData.mesh);
	
    mData.mesh->LockVertexBuffer(0, (VOID**)&vidMem);
    memcpy(vidMem, mVerts, mData.vertCount * sizeof(VERTEX));

    D3DXComputeBoundingSphere((D3DXVECTOR3*)&mVerts,
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

VOID CMesh::Clear(void)
{
	Release();
	mData.vertCount = mData.primCount = mData.indexCount = 0;
	ZeroMemory(&mData, sizeof(RENDERDATA));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
}