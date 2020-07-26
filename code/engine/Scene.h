#pragma once

#include "system.h"
#include "RenderData.h"
#include "Node.h"

class ENGINE_API CScene: public CNode
{
public:
    CScene(LPCSTR modelPath=NULL);
    VOID Release();

    BOOL LoadScene(LPCSTR modelPath, BOOL loadMaterials = TRUE, BOOL optimizeMesh = FALSE);

    inline CNode* GetRootNode() { return mRootNode; }
private:
    CNode* mRootNode;
};