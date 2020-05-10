#include "StdAfx.h"

#include "ReferenceManager.h"

#include "FaceGroup.h"
#include "Material.h"
#include "Mesh.h"
#include "Light.h"
#include "Target.h"

CReferenceContainer<CFaceGroup> CReferenceManager::faceGroups;
CReferenceContainer<CMaterial> CReferenceManager::materials;
CReferenceContainer<CMesh> CReferenceManager::meshes;
CReferenceContainer<CLight> CReferenceManager::lights;
CReferenceContainer<CTarget> CReferenceManager::targets;

VOID CReferenceManager::Release()
{
    faceGroups.Release();
    materials.Release();
    meshes.Release();
    lights.Release();
    targets.Release();
}
