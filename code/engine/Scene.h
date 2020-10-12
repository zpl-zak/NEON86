#pragma once

#include "system.h"
#include "RenderData.h"
#include "Node.h"

class ENGINE_API CScene : public CNode
{
public:
    CScene(LPCSTR modelPath = nullptr);
    VOID Release();

    auto LoadScene(LPCSTR modelPath, BOOL loadMaterials = TRUE, BOOL optimizeMesh = FALSE) -> BOOL;

    auto GetRootNode() const -> CNode* { return mRootNode; }
private:
    CNode* mRootNode;
};
