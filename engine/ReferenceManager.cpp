#include "StdAfx.h"

#include "ReferenceManager.h"

#include "FaceGroup.h"
#include "Material.h"
#include "Mesh.h"

CReferenceContainer<CFaceGroup> CReferenceManager::faceGroups;
CReferenceContainer<CMaterial> CReferenceManager::materials;
CReferenceContainer<CMesh> CReferenceManager::meshes;

void CReferenceManager::Release()
{
    faceGroups.Release();
    materials.Release();
    meshes.Release();
}
