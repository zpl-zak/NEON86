#pragma once

#include "system.h"
#include "RenderData.h"
#include "Node.h"

class CMesh;
class CLight;

class CScene: public CNode
{
public:
    CScene(LPSTR modelPath=NULL);
    VOID Release();

    BOOL LoadScene(LPCSTR modelPath, BOOL loadMaterials = TRUE, BOOL optimizeMesh = FALSE);

    inline CNode* GetRootNode() { return mRootNode; }
private:
    CNode* mRootNode;
};