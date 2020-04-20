#include "StdAfx.h"

#include "ReferenceManager.h"

#include "Mesh.h"
#include "Material.h"
#include "MeshGroup.h"

CReferenceContainer<CMesh> CReferenceManager::meshes;
CReferenceContainer<CMaterial> CReferenceManager::materials;
CReferenceContainer<CMeshGroup> CReferenceManager::meshGroups;

void CReferenceManager::Release()
{
    meshes.Release();
    materials.Release();
    meshGroups.Release();
}
