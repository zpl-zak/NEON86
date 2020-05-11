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

    VOID LoadScene(LPCSTR modelPath, BOOL loadMaterials = TRUE);
    VOID Draw(const D3DXMATRIX& wmat);
    VOID DrawSubset(UINT subset, const D3DXMATRIX& wmat);

    inline CNode* GetRootNode() { return mRootNode; }
private:
    CNode* mRootNode;
};