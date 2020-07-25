#include "stdafx.h"

#include "FaceGroup.h"

#include "Engine.h"
#include "Renderer.h"

#include "Material.h"

CFaceGroup::CFaceGroup(VOID): CAllocable()
{
	ZeroMemory(&mData, sizeof(RENDERDATA));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
}

VOID CFaceGroup::Release(VOID)
{
	if (DelRef())
	{
        mVerts.Release();
        mIndices.Release();
        SAFE_RELEASE(mData.mesh);
        SAFE_RELEASE(mData.mat);
		delete this;
	}
}

VOID CFaceGroup::SetMaterial(CMaterial* tex)
{
	mData.mat = tex;
}

VOID CFaceGroup::AddVertex(const VERTEX& vertex)
{
	mIsDirty = TRUE;
    mVerts.Push(vertex);
}

VOID CFaceGroup::AddIndex(SHORT index)
{
	mIsDirty = TRUE;
	mIndices.Push(index);
}

VOID CFaceGroup::Draw(D3DXMATRIX* mat)
{
	if (!mData.mesh || mIsDirty)
		Build();

	if (mat)
	{
		mData.usesMatrix = TRUE;
		mData.matrix = *mat;
	}
	else mData.usesMatrix = FALSE;

    BOOL isGlobalShadingEnabled = RENDERER->GetLightingState();

    if (isGlobalShadingEnabled && mData.mat)
        RENDERER->EnableLighting(mData.mat->GetMaterialData().Shaded);

    RENDERER->SetDefaultRenderStates();

	if (!RENDERER->UsesMaterialOverride())
		mData.mat->Bind(0);

	RENDERER->DrawMesh(mData);

    RENDERER->EnableLighting(isGlobalShadingEnabled);

	if (!RENDERER->UsesMaterialOverride())
		mData.mat->Unbind(0);
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

	if (mVerts.GetCount() == 0)
		return;

	if (mIndices.GetCount() == 0)
	{
		// Small hack, populate indices from verts
		for (UINT i = 0; i < mVerts.GetCount(); i++) {
			AddIndex(i);
		}
	}

	DWORD numFaces = mIndices.GetCount()/3;
	
	D3DXCreateMesh(numFaces,
		mVerts.GetCount(),
		D3DXMESH_MANAGED,
		meshVertexFormat,
		dev,
		&mData.mesh);

	if (!mData.mesh)
	{
        MessageBoxA(NULL, "Failed to allocate mesh!", "Renderer error", MB_OK);
        ENGINE->Shutdown();
        return;
	}
	
    mData.mesh->LockVertexBuffer(0, (VOID**)&vidMem);
    memcpy(vidMem, mVerts.GetData(), mVerts.GetCount() * sizeof(VERTEX));

    D3DXComputeBoundingSphere((D3DXVECTOR3*)mVerts.GetData(),
        mVerts.GetCount(),
        sizeof(VERTEX),
        &mData.meshOrigin,
        &mData.meshRadius);

	D3DXComputeBoundingBox((D3DXVECTOR3*)mVerts.GetData(),
        mVerts.GetCount(),
        sizeof(VERTEX),
		(D3DXVECTOR3*)&mData.meshBounds[0],
		(D3DXVECTOR3*)&mData.meshBounds[1]
	);

	mData.mesh->UnlockVertexBuffer();

    mData.mesh->LockIndexBuffer(0, (VOID**)&vidMem);
    memcpy(vidMem, mIndices.GetData(), mIndices.GetCount() * sizeof(SHORT));
    mData.mesh->UnlockIndexBuffer();

	mIsDirty = FALSE;
}

VOID CFaceGroup::Clear(VOID)
{
	ZeroMemory(&mData, sizeof(RENDERDATA));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
    mVerts.Release();
    mIndices.Release();
    SAFE_RELEASE(mData.mesh);
    SAFE_RELEASE(mData.mat);
	mIsDirty = FALSE;
}

CFaceGroup* CFaceGroup::Clone()
{
	CFaceGroup* clonedFG = new CFaceGroup();
	clonedFG->SetMaterial(mData.mat);
	mData.mat->AddRef();

	for (auto v : mVerts)
		clonedFG->AddVertex(v);

	for (auto i : mIndices)
		clonedFG->AddIndex(i);

	return clonedFG;
}
