#include "stdafx.h"

#include "FaceGroup.h"

#include "Engine.h"
#include "Renderer.h"

#include "Material.h"

CFaceGroup::CFaceGroup()
{
    ZeroMemory(&mData, sizeof(RENDERDATA));
    mData.kind = PRIMITIVEKIND_TRIANGLELIST;
    mIsDirty = FALSE;
}

CFaceGroup::~CFaceGroup()
{
    Release();
}

void CFaceGroup::Release()
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

void CFaceGroup::SetMaterial(CMaterial* tex)
{
    mData.mat = tex;
}

void CFaceGroup::AddVertex(const VERTEX& vertex)
{
    mIsDirty = TRUE;
    mVerts.Push(vertex);
}

void CFaceGroup::AddIndex(short index)
{
    mIsDirty = TRUE;
    mIndices.Push(index);
}

void CFaceGroup::Draw(D3DXMATRIX* mat)
{
    if ((mData.mesh == nullptr) || mIsDirty)
    {
        Build();
    }

    if (mat != nullptr)
    {
        mData.usesMatrix = TRUE;
        mData.matrix = *mat;
    }
    else
    {
        mData.usesMatrix = FALSE;
    }

    bool isGlobalShadingEnabled = RENDERER->GetLightingState();

    if (isGlobalShadingEnabled && (mData.mat != nullptr))
    {
        RENDERER->EnableLighting(mData.mat->GetMaterialData().Shaded);
    }

    RENDERER->SetDefaultRenderStates();

    if (!RENDERER->UsesMaterialOverride())
    {
        if (mData.mat)
            mData.mat->Bind(0);
    }

    RENDERER->DrawMesh(mData);

    RENDERER->EnableLighting(isGlobalShadingEnabled);

    if (!RENDERER->UsesMaterialOverride())
    {
        if (mData.mat)
            mData.mat->Unbind(0);
    }
}

void CFaceGroup::CalculateNormals() const
{
    if (mData.mesh == nullptr)
    {
        MessageBoxA(nullptr, "Mesh is not built yet, you can not calculate normals!", "Mesh error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    D3DXComputeNormals(mData.mesh, nullptr);
}

void CFaceGroup::Build()
{
    LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
    void* vidMem = nullptr;
    SAFE_RELEASE(mData.mesh);

    if (mVerts.GetCount() == 0)
    {
        return;
    }

    if (mIndices.GetCount() == 0)
    {
        // Small hack, populate indices from verts
        for (unsigned int i = 0; i < mVerts.GetCount(); i++)
        {
            AddIndex(i);
        }
    }

    DWORD numFaces = mIndices.GetCount() / 3;

    D3DXCreateMesh(numFaces,
                   mVerts.GetCount(),
                   D3DXMESH_MANAGED,
                   meshVertexFormat,
                   dev,
                   &mData.mesh);

    if (mData.mesh == nullptr)
    {
        MessageBoxA(nullptr, "Failed to allocate mesh!", "Renderer error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    mData.mesh->LockVertexBuffer(0, static_cast<void**>(&vidMem));
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

    mData.mesh->LockIndexBuffer(0, static_cast<void**>(&vidMem));
    memcpy(vidMem, mIndices.GetData(), mIndices.GetCount() * sizeof(short));
    mData.mesh->UnlockIndexBuffer();

    mIsDirty = FALSE;
}

void CFaceGroup::Clear()
{
    ZeroMemory(&mData, sizeof(RENDERDATA));
    mData.kind = PRIMITIVEKIND_TRIANGLELIST;
    mVerts.Release();
    mIndices.Release();
    SAFE_RELEASE(mData.mesh);
    SAFE_RELEASE(mData.mat);
    mIsDirty = FALSE;
}

auto CFaceGroup::Clone() -> CFaceGroup*
{
    auto* clonedFG = new CFaceGroup();
    clonedFG->SetMaterial(mData.mat);
    mData.mat->AddRef();

    for (auto v : mVerts)
    {
        clonedFG->AddVertex(v);
    }

    for (auto i : mIndices)
    {
        clonedFG->AddIndex(i);
    }

    return clonedFG;
}
