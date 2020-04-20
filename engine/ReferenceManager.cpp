#include "StdAfx.h"

#include "ReferenceManager.h"

#include "Mesh.h"
#include "Material.h"

CReferenceContainer<CMesh> CReferenceManager::meshes;
CReferenceContainer<CMaterial> CReferenceManager::materials;

void CReferenceManager::Release()
{
    meshes.Release();
    materials.Release();
}
