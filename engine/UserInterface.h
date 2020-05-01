#pragma once

#include "system.h"

#define UI CEngine::the()->GetUI()

class ENGINE_API CUserInterface
{
public:
    CUserInterface();
    BOOL Release(VOID);
    VOID Update(FLOAT dt);
    VOID Render(VOID);
    LRESULT ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    VOID DebugPanel(VOID);
};

