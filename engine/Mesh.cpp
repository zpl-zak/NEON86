#include "stdafx.h"

#include "Mesh.h"
#include "NeonEngine.h"

#include "Texture.h"

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
	SAFE_RELEASE(mData.vertBuffer);
	SAFE_RELEASE(mData.indexBuffer);
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
	if (!mData.vertBuffer || mIsDirty)
		Build();

	if (mData.tex)
		mData.tex->Bind(mData.stage);

	if (mat)
	{
		mData.usesMatrix = TRUE;
		mData.matrix = *mat;
	}
	else mData.usesMatrix = FALSE;

	RENDERER->PushCommand(RENDERKIND_POLYGON, mData);

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

	dev->CreateVertexBuffer(mData.vertCount*sizeof(VERTEX),
							0,
							NEONFVF,
							D3DPOOL_MANAGED,
							&mData.vertBuffer,
							NULL);

	mData.vertBuffer->Lock(0, 0, (VOID**)&vidMem, 0);
	memcpy(vidMem, mVerts, mData.vertCount*sizeof(VERTEX));
	mData.vertBuffer->Unlock();

	if (mData.indexCount > 0)
	{
		dev->CreateIndexBuffer(mData.indexCount*sizeof(SHORT),
							   0,
							   D3DFMT_INDEX16,
							   D3DPOOL_MANAGED,
							   &mData.indexBuffer,
							   NULL);

		mData.indexBuffer->Lock(0, 0, (VOID**)&vidMem, 0);
		memcpy(vidMem, mIndices, mData.indexCount*sizeof(SHORT));
		mData.indexBuffer->Unlock();
	}

	mData.primCount = ((mData.indexCount > 0) ? mData.indexCount : mData.vertCount)/3;
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