#pragma once

#include "system.h"

#include <assimp/matrix4x4.h>

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

class CSceneLoader 
{
public:
    static VOID LoadNodesRecursively(const aiScene* impScene, const aiNode* impNode, CScene* scene, CNode* node, BOOL loadMaterials);
    static VOID LoadScene(const aiScene* impScene, CScene* scene, BOOL loadMaterials);
    static CFaceGroup* LoadFaceGroup(const aiScene* scene, const aiMesh* mesh, BOOL loadMaterials);
    static CLight* LoadLight(const aiNode* impNode, const aiLight* impLight);
    static aiMatrix4x4 ComputeFinalTransformation(const aiNode* node);
private:
    static VOID LoadTextureMap(const aiScene* scene, const aiMaterial* mat, CMaterial* newMaterial, UINT slot, UINT texType);
};