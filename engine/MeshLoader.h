#pragma once

#include "system.h"

class CFaceGroup;
class CMaterial;
struct aiScene;
struct aiMesh;
struct aiMaterial;

class CMeshLoader 
{
public:
    static CFaceGroup* LoadNode(const aiScene* scene, const aiMesh* mesh, UINT texFiltering);

private:
    static void LoadTextureMap(const aiScene* scene, const aiMaterial* mat, CMaterial* newMaterial, UINT slot, UINT texType);
};