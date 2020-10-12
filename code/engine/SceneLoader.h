#pragma once

#include "system.h"

class CFaceGroup;
class CMaterial;
class CScene;
class CNode;
class CLight;
struct aiScene;
struct aiMesh;
struct aiNode;
struct aiLight;
struct aiMaterial;

class ENGINE_API CSceneLoader
{
public:
    static VOID LoadNodesRecursively(const aiScene* impScene, const aiNode* impNode, CScene* scene, CNode* node,
                                     BOOL loadMaterials);
    static auto LoadScene(LPCSTR modelPath, CScene* scene, BOOL loadMaterials, BOOL optimizeMeshes) -> BOOL;
    static auto LoadFaceGroup(const aiScene* scene, const aiMesh* mesh, BOOL loadMaterials) -> CFaceGroup*;
    static auto LoadLight(const aiNode* impNode, const aiLight* impLight) -> CLight*;
    static VOID LoadTextureMap(const aiScene* scene, const aiMaterial* mat, CMaterial* newMaterial, UINT slot,
                               UINT texType);
};
