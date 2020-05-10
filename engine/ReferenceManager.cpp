#include "StdAfx.h"

#include "ReferenceManager.h"

#include "FaceGroup.h"
#include "Material.h"
#include "Mesh.h"
#include "Light.h"

CReferenceContainer<CFaceGroup> CReferenceManager::faceGroups;
CReferenceContainer<CMaterial> CReferenceManager::materials;
CReferenceContainer<CMesh> CReferenceManager::meshes;
CReferenceContainer<CLight> CReferenceManager::lights;

VOID CReferenceManager::Release()
{
    faceGroups.Release();
    materials.Release();
    meshes.Release();
    lights.Release();
}
