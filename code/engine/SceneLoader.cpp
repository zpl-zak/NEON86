#include "StdAfx.h"
#include "SceneLoader.h"
#include "FaceGroup.h"
#include "Scene.h"
#include "Material.h"
#include "Mesh.h"
#include "Light.h"
#include "ReferenceManager.h"

#include "Engine.h"
#include "FileSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
#include <assimp/postprocess.h>
#include <sstream>

#ifndef _DEBUG
#pragma comment (lib, "assimp.lib")
#else
#pragma comment (lib, "assimpd.lib")
#endif

#define MESHIMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded |\
    aiProcess_Triangulate |\
    aiProcess_CalcTangentSpace |\
    aiProcess_SplitLargeMeshes |\
    0

extern auto ComputeFinalTransformation(const aiNode* node) -> aiMatrix4x4;

void CSceneLoader::LoadNodesRecursively(const aiScene* impScene, const aiNode* impNode, CScene* scene, CNode* node,
                                        bool loadMaterials)
{
    aiMatrix4x4 mat = impNode->mTransformation;
    mat = mat.Transpose();

    auto* newNode = new CNode(*(D3DXMATRIX*)&mat, impNode->mName.C_Str());
    node->AddNode(newNode);

    if (node != scene)
    {
        scene->AddNode(newNode);
    }

    newNode->SetParent(node);

    if (impNode->mMetaData != nullptr)
    {
        for (unsigned int i = 0; i < impNode->mMetaData->mNumProperties; ++i)
        {
            aiString k = impNode->mMetaData->mKeys[i];
            const aiMetadataEntry* e = &impNode->mMetaData->mValues[i];
            std::string data;

            switch (e->mType)
            {
            case AI_AISTRING:
                data = (*static_cast<aiString*>(e->mData)).C_Str();
                break;

            case AI_BOOL:
            case AI_DOUBLE:
            case AI_FLOAT:
            case AI_INT32:
            case AI_UINT64:
                data = std::to_string(*static_cast<DOUBLE*>(e->mData));
                break;

            case AI_AIVECTOR3D:
                {
                    aiVector3D a = *static_cast<aiVector3D*>(e->mData);
                    std::stringstream ss;
                    ss << a.x << "," << a.y << "," << a.z;
                    data = ss.str();
                }
                break;
            }

            newNode->SetMetadata(static_cast<LPCSTR>(k.C_Str()), data.c_str());
        }
    }

    aiString lastMeshName = aiString("(unknown)");
    CMesh* lastMesh = nullptr;

    for (unsigned int i = 0; i < impNode->mNumMeshes; ++i)
    {
        const aiMesh* impMesh = impScene->mMeshes[impNode->mMeshes[i]];

        if ((lastMesh == nullptr) || lastMeshName != impMesh->mName)
        {
            if (lastMesh != nullptr)
            {
                lastMesh->SetName(lastMeshName.C_Str());
                scene->AddMesh(lastMesh);
                newNode->AddMesh(lastMesh);
            }

            lastMesh = new CMesh();
            lastMeshName = impMesh->mName;
        }

        static D3DXMATRIX identityMat;
        D3DXMatrixIdentity(&identityMat);

        lastMesh->AddFaceGroup(LoadFaceGroup(impScene, impMesh, loadMaterials), *static_cast<D3DMATRIX*>(&identityMat));
    }

    if (lastMesh != nullptr)
    {
        lastMesh->SetName(lastMeshName.C_Str());
        scene->AddMesh(lastMesh);
        newNode->AddMesh(lastMesh);
    }

    // Load light
    for (unsigned int i = 0; i < impScene->mNumLights; i++)
    {
        const aiLight* impLight = impScene->mLights[i];
        if (strcmp(impNode->mName.C_Str(), impLight->mName.C_Str()) == 0)
        {
            CLight* lit = LoadLight(impNode, impLight);
            scene->AddLight(lit);
            newNode->AddLight(lit);
        }
    }

    // Iterate over children
    for (unsigned int i = 0; i < impNode->mNumChildren; i++)
    {
        LoadNodesRecursively(impScene, impNode->mChildren[i], scene, newNode, loadMaterials);
    }
}

auto CSceneLoader::LoadScene(LPCSTR modelPath, CScene* scene, bool loadMaterials, bool optimizeMeshes) -> bool
{
    Assimp::Importer imp;
    imp.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 32762);

    DWORD meshFlags = MESHIMPORT_FLAGS;

    if (optimizeMeshes)
    {
        meshFlags |= aiProcess_PreTransformVertices
                | aiProcess_RemoveRedundantMaterials;
    }

    const aiScene* model = imp.ReadFile(FILESYSTEM->ResourcePath(modelPath), meshFlags);

    if (model == nullptr)
    {
        return FALSE;
    }

    LoadNodesRecursively(model, model->mRootNode, scene, scene, loadMaterials);
    return TRUE;
}

auto CSceneLoader::LoadFaceGroup(const aiScene* scene, const aiMesh* mesh, bool loadMaterials) -> CFaceGroup*
{
    auto* newFGroup = new CFaceGroup();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        VERTEX vert = {0};

        const aiVector3D pos = mesh->mVertices[i];
        aiVector3D nm;
        aiVector3D ta;
        aiVector3D tb;

        if (mesh->HasNormals())
        {
            nm = mesh->mNormals[i];
        }

        if (mesh->HasTangentsAndBitangents())
        {
            ta = mesh->mTangents[i];
            tb = mesh->mBitangents[i];
        }

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
                static_cast<BYTE>(tc.a * 255.0F),
                static_cast<BYTE>(tc.r * 255.0F),
                static_cast<BYTE>(tc.g * 255.0F),
                static_cast<BYTE>(tc.b * 255.0F)
            );
        }
        else
        {
            vert.color = D3DCOLOR_XRGB(255, 255, 255);
        }

        newFGroup->AddVertex(vert);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace face = mesh->mFaces[i];

        newFGroup->AddIndex(face.mIndices[0]);
        newFGroup->AddIndex(face.mIndices[1]);
        newFGroup->AddIndex(face.mIndices[2]);
    }

    if (mesh->mMaterialIndex < scene->mNumMaterials && loadMaterials)
    {
        const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        auto* newMaterial = new CMaterial();

        LoadTextureMap(scene, mat, newMaterial, TEXTURESLOT_ALBEDO, aiTextureType_DIFFUSE);
        LoadTextureMap(scene, mat, newMaterial, TEXTURESLOT_SPECULAR, aiTextureType_SPECULAR);
        LoadTextureMap(scene, mat, newMaterial, TEXTURESLOT_NORMAL, aiTextureType_NORMALS);
        LoadTextureMap(scene, mat, newMaterial, TEXTURESLOT_DISPLACE, aiTextureType_DISPLACEMENT);

        aiColor4D diffuse;
        mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        newMaterial->SetDiffuse(D3DCOLORVALUE{diffuse.r, diffuse.g, diffuse.b});

        aiColor4D specular;
        float power;
        mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        mat->Get(AI_MATKEY_SHININESS, power);

        newMaterial->SetSpecular(D3DCOLORVALUE{specular.r, specular.g, specular.b});
        newMaterial->SetPower(power * 5.0F);

        /* aiColor4D ambient;
         mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
         newMaterial->SetAmbient(D3DCOLORVALUE{ ambient.r, ambient.g, ambient.b });*/

        aiColor4D emissive;
        mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        newMaterial->SetEmission(D3DCOLORVALUE{emissive.r, emissive.g, emissive.b});

        float opacity = 1.0F;
        mat->Get(AI_MATKEY_OPACITY, opacity);
        newMaterial->SetOpacity(opacity);

        newFGroup->SetMaterial(newMaterial);
    }

    return newFGroup;
}

auto CSceneLoader::LoadLight(const aiNode* impNode, const aiLight* impLight) -> CLight*
{
    auto* lit = new CLight();

    aiMatrix4x4 mat = ComputeFinalTransformation(impNode).Transpose();

    aiVector3D pos;
    aiVector3D dir;
    aiQuaternion rot;
    mat.DecomposeNoScaling(rot, pos);

    pos = impLight->mPosition + pos;
    dir = rot.GetMatrix() * impLight->mDirection;

    lit->SetPosition(EXPAND_VEC(pos));
    lit->SetDirection(EXPAND_VEC(dir));

    lit->SetDiffuse(EXPAND_COLX(impLight->mColorDiffuse));
    lit->SetSpecular(EXPAND_COLX(impLight->mColorSpecular));
    lit->SetAttenuation(impLight->mAttenuationConstant,
                        impLight->mAttenuationLinear,
                        impLight->mAttenuationQuadratic);
    lit->SetInnerAngle(impLight->mAngleInnerCone);
    lit->SetOuterAngle(impLight->mAngleOuterCone);

    switch (impLight->mType)
    {
    case aiLightSource_POINT:
        lit->SetType(D3DLIGHT_POINT);
        break;
    case aiLightSource_DIRECTIONAL:
        lit->SetType(D3DLIGHT_DIRECTIONAL);
        break;
    case aiLightSource_SPOT:
        lit->SetType(D3DLIGHT_SPOT);
        break;
    }

    lit->SetName(impNode->mName.C_Str());
    return lit;
}

void CSceneLoader::LoadTextureMap(const aiScene* scene, const aiMaterial* mat, CMaterial* newMaterial,
                                  unsigned int slot,
                                  unsigned int texType)
{
    aiString path;
    mat->GetTexture(static_cast<aiTextureType>(texType), 0, &path);
    const aiTexture* tex = scene->GetEmbeddedTexture(path.C_Str());

    if (tex != nullptr)
    {
        if (tex->mHeight != 0)
        {
            newMaterial->CreateTextureForSlot(slot, nullptr, tex->mWidth, tex->mHeight);
            newMaterial->UploadARGB(slot, tex->pcData, sizeof(aiTexel) * tex->mWidth * tex->mHeight);
        }
        else
        {
            newMaterial->CreateEmbeddedTextureForSlot(slot, tex->pcData, tex->mWidth);
        }
    }
    else if (FILESYSTEM->Exists((LPSTR)path.C_Str()))
    {
        newMaterial->CreateTextureForSlot(slot, (LPSTR)path.C_Str());
    }

    if (slot != TEXTURESLOT_ALBEDO || (newMaterial->GetTextureHandle(slot) == nullptr))
    {
        return;
    }

    D3DSURFACE_DESC a;
    newMaterial->GetTextureHandle(slot)->GetLevelDesc(0, &a);

    auto* buf = static_cast<unsigned int*>(newMaterial->Lock(slot));

    for (unsigned int i = 0; i < (a.Width * a.Height); i++)
    {
        if (((buf[i] << 24) & 0xFF) < 1.0F)
        {
            newMaterial->SetAlphaIsTransparency(TRUE);
            newMaterial->SetEnableAlphaTest(TRUE);
            newMaterial->SetAlphaRef(127);
            break;
        }
    }

    newMaterial->Unlock(slot);
}

static auto ComputeFinalTransformation(const aiNode* node) -> aiMatrix4x4
{
    if (node->mParent == nullptr)
    {
        return {};
    }

    return node->mTransformation * ComputeFinalTransformation(node->mParent);
}
