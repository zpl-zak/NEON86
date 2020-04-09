#include "stdafx.h"

#include "MeshBuilder.h"
#include "NeonEngine.h"

MeshBuilder::MeshBuilder(void)
{
	ZeroMemory(&mData, sizeof(RENDERDATA));
	ZeroMemory(mVerts, sizeof(mVerts));
	ZeroMemory(mIndices, sizeof(mIndices));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
}

VOID MeshBuilder::Release(void)
{
	SAFE_RELEASE(mData.vertBuffer);
	SAFE_RELEASE(mData.indexBuffer);
}

VOID MeshBuilder::SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 tex)
{
	mData.stage = stage;
	mData.tex = tex;
}

VOID MeshBuilder::AddVertex(const VERTEX& vertex)
{
	mVerts[mData.vertCount++] = vertex;
}

VOID MeshBuilder::AddIndex(SHORT index)
{
	mIndices[mData.indexCount++] = index;
}

VOID MeshBuilder::Draw(void)
{
	if (!mData.vertBuffer)
		Build();

	if (mData.tex)
		RENDERER->PushTexture(mData.stage, mData.tex);

	mData.primCount = ((mData.indexCount > 0) ? mData.indexCount : mData.vertCount)/3;
	RENDERER->PushCommand(RENDERKIND_POLYGON, mData);

	if (mData.tex)
		RENDERER->PushTexture(mData.stage, NULL);
}

VOID MeshBuilder::Build(void)
{
	LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
	VOID *vidMem = NULL;
	Release();

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
}
