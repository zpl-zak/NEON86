#include "stdafx.h"

#include "MeshBuilder.h"
#include "NeonEngine.h"

CMeshBuilder::CMeshBuilder(void)
{
	ZeroMemory(&mData, sizeof(RENDERDATA));
	ZeroMemory(mVerts, sizeof(mVerts));
	ZeroMemory(mIndices, sizeof(mIndices));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
}

VOID CMeshBuilder::Release(void)
{
	SAFE_RELEASE(mData.vertBuffer);
	SAFE_RELEASE(mData.indexBuffer);
}

VOID CMeshBuilder::SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 tex)
{
	mData.stage = stage;
	mData.tex = tex;
}

VOID CMeshBuilder::AddVertex(const VERTEX& vertex)
{
	mIsDirty = TRUE;
	mVerts[mData.vertCount++] = vertex;
}

VOID CMeshBuilder::AddIndex(SHORT index)
{
	mIsDirty = TRUE;
	mIndices[mData.indexCount++] = index;
}

VOID CMeshBuilder::Draw(void)
{
	if (!mData.vertBuffer || mIsDirty)
		Build();

	if (mData.tex)
		RENDERER->PushTexture(mData.stage, mData.tex);

	RENDERER->PushCommand(RENDERKIND_POLYGON, mData);

	if (mData.tex)
		RENDERER->PushTexture(mData.stage, NULL);
}

VOID CMeshBuilder::Build(void)
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

VOID CMeshBuilder::Clear(void)
{
	Release();
	mData.vertCount = mData.primCount = mData.indexCount = 0;
	ZeroMemory(&mData, sizeof(RENDERDATA));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
}