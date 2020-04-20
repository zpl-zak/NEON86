#include "stdafx.h"

#include "MeshGroup.h"
#include "Mesh.h"
#include "NeonEngine.h"
#include "FileSystem.h"
#include "Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CMeshGroup::CMeshGroup(void)
{
    mCount = 0;
    mCapacity = 4;
    mMeshes = (CMesh**)malloc(mCapacity * sizeof(CMesh*));
    mTransforms = (D3DXMATRIX*)malloc(mCapacity * sizeof(D3DXMATRIX));
    D3DXMatrixIdentity(&identityMat);
}

VOID CMeshGroup::Release(void)
{
    SAFE_FREE(mMeshes);
    SAFE_FREE(mTransforms);
    mCount = mCapacity = 0;
}

VOID CMeshGroup::AddMesh(CMesh* mesh, const D3DXMATRIX& mat)
{
    if (!mesh)
        return;

    if (mCount >= mCapacity)
    {
        mCapacity += 4;

        mMeshes = (CMesh**)realloc(mMeshes, mCapacity * sizeof(CMesh*));
        mTransforms = (D3DXMATRIX*)realloc(mTransforms, mCapacity * sizeof(D3DXMATRIX));

        if (!mMeshes || !mTransforms)
        {
            MessageBoxA(NULL, "Can't add mesh to mesh group!", "Out of memory error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
    }

    mMeshes[mCount] = mesh;
    mTransforms[mCount] = mat;
    mCount++;
}

#define MESHIMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded |\
    aiProcess_Triangulate |\
    aiProcess_CalcTangentSpace |\
    aiProcess_FlipUVs |\
    aiProcess_JoinIdenticalVertices |\
    aiProcess_PreTransformVertices

VOID CMeshGroup::LoadMesh(LPCSTR modelName, UINT texFiltering)
{
    Assimp::Importer imp;

    const aiScene* model = imp.ReadFile(FILESYSTEM->ResourcePath(RESOURCEKIND_USER, modelName), MESHIMPORT_FLAGS);

    if (!model)
    {
        MessageBoxA(NULL, "Mesh import failed!", "Mesh error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    for (UINT i = 0; i < model->mNumMeshes; i++)
    {
        CMesh* mesh = LoadNode(model, model->mMeshes[i], texFiltering);
        AddMesh(mesh, identityMat);
    }
}

VOID CMeshGroup::Draw(const D3DXMATRIX& wmat)
{
    if (!mCount)
        return;

    for (UINT i = 0; i < mCount; i++)
    {
        D3DXMATRIX mat = mTransforms[i] * wmat;
        mMeshes[i]->Draw(&mat);
    }
}

VOID CMeshGroup::Clear(void)
{
    mCount = 0;
}

CMesh* CMeshGroup::LoadNode(const aiScene* scene, const aiMesh* mesh, UINT texFiltering)
{
    CMesh* newMesh = new CMesh();
    CReferenceManager::TrackRef(newMesh);

    for (UINT i=0; i<mesh->mNumVertices;i++)
    {
        VERTEX vert = { 0 };

        const aiVector3D pos = mesh->mVertices[i];
        const aiVector3D uv = mesh->mTextureCoords[0][i];
        const aiVector3D nm = mesh->mNormals[i];

        vert.x = pos.x;
        vert.y = pos.y;
        vert.z = pos.z;

        vert.nx = nm.x;
        vert.ny = nm.y;
        vert.nz = nm.z;

        vert.su = uv.x;
        vert.tv = uv.y;

        vert.color = D3DCOLOR_XRGB(255,255,255);

        newMesh->AddVertex(vert);
    }

    for (UINT i=0; i<mesh->mNumFaces; i++)
    {
        const aiFace face = mesh->mFaces[i];

        newMesh->AddIndex(face.mIndices[0]);
        newMesh->AddIndex(face.mIndices[1]);
        newMesh->AddIndex(face.mIndices[2]);
    }

    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        CMaterial* newMaterial = NULL;

        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        const aiTexture* tex = scene->GetEmbeddedTexture(path.C_Str());

        if (tex)
        {
            if (tex->mHeight != 0)
            {
                newMaterial = new CMaterial(NULL, tex->mWidth, tex->mHeight);
                newMaterial->UploadRGB888(tex->pcData, sizeof(aiTexel) * tex->mWidth * tex->mHeight);
            }
            else
            {
                newMaterial = new CMaterial(tex->pcData, tex->mWidth);
            }
        }
        else
        {
            newMaterial = new CMaterial((LPSTR)path.C_Str());
        }

        if (newMaterial)
        {
            newMaterial->SetSamplerState(SAMPLERSTATE_MAGFILTER, texFiltering);
            newMaterial->SetSamplerState(SAMPLERSTATE_MIPFILTER, texFiltering);
            newMaterial->SetSamplerState(SAMPLERSTATE_MINFILTER, texFiltering);

            aiColor4D diffuse;
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
            newMaterial->SetDiffuse(D3DCOLORVALUE{ diffuse.r, diffuse.g, diffuse.b });

            aiColor4D specular;
            mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
            newMaterial->SetSpecular(D3DCOLORVALUE{ specular.r, specular.g, specular.b });
            newMaterial->SetPower(20.0F);

            aiColor4D ambient;
            mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
            newMaterial->SetAmbient(D3DCOLORVALUE{ ambient.r, ambient.g, ambient.b });

            aiColor4D emissive;
            mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
            newMaterial->SetEmission(D3DCOLORVALUE{ emissive.r, emissive.g, emissive.b });

            CReferenceManager::TrackRef(newMaterial);
        }

        newMesh->SetTexture(0, newMaterial);
    }

    return newMesh;
}
