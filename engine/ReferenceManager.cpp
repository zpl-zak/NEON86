#include "StdAfx.h"

#include "ReferenceManager.h"

#include "FaceGroup.h"
#include "Material.h"
#include "Mesh.h"

CReferenceContainer<CFaceGroup> CReferenceManager::meshes;
CReferenceContainer<CMaterial> CReferenceManager::materials;
CReferenceContainer<CMesh> CReferenceManager::meshGroups;

void CReferenceManager::Release()
{
    meshes.Release();
    materials.Release();
    meshGroups.Release();
}
