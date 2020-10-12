#pragma once

#include "system.h"
#include "RenderData.h"
#include "Node.h"

class ENGINE_API CScene : public CNode
{
public:
    CScene(LPCSTR modelPath = nullptr);
    void Release();

    auto LoadScene(LPCSTR modelPath, bool loadMaterials = TRUE, bool optimizeMesh = FALSE) -> bool;

    auto GetRootNode() const -> CNode* { return mRootNode; }
private:
    CNode* mRootNode;
};
