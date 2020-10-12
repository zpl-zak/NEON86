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
    static void LoadNodesRecursively(const aiScene* impScene, const aiNode* impNode, CScene* scene, CNode* node,
                                     bool loadMaterials);
    static auto LoadScene(LPCSTR modelPath, CScene* scene, bool loadMaterials, bool optimizeMeshes) -> bool;
    static auto LoadFaceGroup(const aiScene* scene, const aiMesh* mesh, bool loadMaterials) -> CFaceGroup*;
    static auto LoadLight(const aiNode* impNode, const aiLight* impLight) -> CLight*;
    static void LoadTextureMap(const aiScene* scene, const aiMaterial* mat, CMaterial* newMaterial, unsigned int slot,
                               unsigned int texType);
};
