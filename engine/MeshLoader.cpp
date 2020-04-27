#include "StdAfx.h"
#include "MeshLoader.h"
#include "FaceGroup.h"
#include "Material.h"
#include "NeonEngine.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

CFaceGroup* CMeshLoader::LoadNode(const aiScene* scene, const aiMesh* mesh, BOOL loadMaterials)
{
    CFaceGroup* newFGroup = new CFaceGroup();
    CReferenceManager::TrackRef(newFGroup);

    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        VERTEX vert = { 0 };

        const aiVector3D pos = mesh->mVertices[i];
        const aiVector3D nm = mesh->mNormals[i];
        const aiVector3D ta = mesh->mTangents[i];
        const aiVector3D tb = mesh->mBitangents[i];

        vert.x = pos.x;
        vert.y = pos.y;
        vert.z = pos.z;

        vert.nx = nm.x;
        vert.ny = nm.y;
        vert.nz = nm.z;

        if (mesh->HasTextureCoords(0))
        {
            const aiVector3D uv = mesh->mTextureCoords[0][i];
            vert.su = uv.x;
            vert.tv = uv.y;
        }

        if (mesh->HasTextureCoords(1))
        {
            const aiVector3D uv = mesh->mTextureCoords[1][i];
            vert.su2 = uv.x;
            vert.tv2 = uv.y;
        }

        vert.tx = ta.x;
        vert.ty = ta.y;
        vert.tz = ta.z;

        vert.bx = tb.x;
        vert.by = tb.y;
        vert.bz = tb.z;

        if (mesh->HasVertexColors(0))
        {
            const aiColor4D tc = mesh->mColors[0][i];

            vert.color = D3DCOLOR_ARGB(
                (BYTE)(tc.a * 255.0f),
                (BYTE)(tc.r * 255.0f),
                (BYTE)(tc.g * 255.0f),
                (BYTE)(tc.b * 255.0f)
            );
        }
        else vert.color = D3DCOLOR_XRGB(255, 255, 255);

        newFGroup->AddVertex(vert);
    }

    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace face = mesh->mFaces[i];

        newFGroup->AddIndex(face.mIndices[0]);
        newFGroup->AddIndex(face.mIndices[1]);
        newFGroup->AddIndex(face.mIndices[2]);
    }

    if (mesh->mMaterialIndex < scene->mNumMaterials && loadMaterials)
    {
        const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        CMaterial* newMaterial = new CMaterial();

        LoadTextureMap(scene, mat, newMaterial, TEXTURESLOT_ALBEDO, aiTextureType_DIFFUSE);
        LoadTextureMap(scene, mat, newMaterial, TEXTURESLOT_SPECULAR, aiTextureType_SPECULAR);
        LoadTextureMap(scene, mat, newMaterial, TEXTURESLOT_NORMAL, aiTextureType_NORMALS);
        LoadTextureMap(scene, mat, newMaterial, TEXTURESLOT_DISPLACE, aiTextureType_DISPLACEMENT);

        aiColor4D diffuse;
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        newMaterial->SetDiffuse(D3DCOLORVALUE{ diffuse.r, diffuse.g, diffuse.b });

        aiColor4D specular;
        float power;
        mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        mat->Get(AI_MATKEY_SHININESS, power);

        newMaterial->SetSpecular(D3DCOLORVALUE{ specular.r, specular.g, specular.b });
        newMaterial->SetPower(power * 5.0f);

        aiColor4D ambient;
        mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        newMaterial->SetAmbient(D3DCOLORVALUE{ ambient.r, ambient.g, ambient.b });

        aiColor4D emissive;
        mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        newMaterial->SetEmission(D3DCOLORVALUE{ emissive.r, emissive.g, emissive.b });

        CReferenceManager::TrackRef(newMaterial);
        newFGroup->SetMaterial(0, newMaterial);
    }

    return newFGroup;
}

void CMeshLoader::LoadTextureMap(const aiScene* scene, const aiMaterial* mat, CMaterial* newMaterial, UINT slot, UINT texType)
{
    aiString path;
    mat->GetTexture((aiTextureType)texType, 0, &path);
    const aiTexture* tex = scene->GetEmbeddedTexture(path.C_Str());

    if (tex)
    {
        if (tex->mHeight != 0)
        {
            newMaterial->CreateTextureForSlot(slot, NULL, tex->mWidth, tex->mHeight);
            newMaterial->UploadARGB(slot, tex->pcData, sizeof(aiTexel) * tex->mWidth * tex->mHeight);
        }
        else
        {
            newMaterial->CreateEmbeddedTextureForSlot(slot, tex->pcData, tex->mWidth);
        }
    }
    else if (FILESYSTEM->Exists(RESOURCEKIND_USER, (LPSTR)path.C_Str()))
    {
        newMaterial->CreateTextureForSlot(slot, (LPSTR)path.C_Str());
    }
}
