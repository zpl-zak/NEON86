#include "StdAfx.h"
#include "MeshLoader.h"
#include "Mesh.h"
#include "Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

CMesh* CMeshLoader::LoadNode(const aiScene* scene, const aiMesh* mesh, UINT texFiltering)
{
    CMesh* newMesh = new CMesh();
    CReferenceManager::TrackRef(newMesh);

    for (UINT i = 0; i < mesh->mNumVertices; i++)
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

        vert.color = D3DCOLOR_XRGB(255, 255, 255);

        newMesh->AddVertex(vert);
    }

    for (UINT i = 0; i < mesh->mNumFaces; i++)
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
