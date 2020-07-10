#pragma once

#include "system.h"
#include <string>

#define UI CEngine::the()->GetUI()

class ENGINE_API CGameEditor
{
public:
    CGameEditor();
    BOOL Release(VOID);
    VOID Update(FLOAT dt);
    VOID Render(VOID);
    LRESULT ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    VOID ClearErrorWindow();
    VOID PushErrorMessage(LPCSTR err);
    ID3DXSprite* GetTextSurface() { return mTextSurface; }
private:
    VOID DebugPanel(VOID);
    std::string FormatBytes(UINT64 bytes);

#if _DEBUG
    // Error handling
    BOOL mShowError;
    std::string *mErrorMessage;
#endif

    ID3DXSprite* mTextSurface;
};

