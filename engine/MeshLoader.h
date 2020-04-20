#pragma once

#include "system.h"

class CMesh;
struct aiScene;
struct aiMesh;

class CMeshLoader 
{
public:
    static CMesh* LoadNode(const aiScene* scene, const aiMesh* mesh, UINT texFiltering);
};