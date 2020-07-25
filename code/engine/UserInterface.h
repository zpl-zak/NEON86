#pragma once

#include "system.h"

#define UI CEngine::the()->GetUI()

#include <functional>

using Draw2DHook = std::function<void()>;

class ENGINE_API CUserInterface
{
public:
    CUserInterface();
    BOOL Release(VOID);
    VOID Update(FLOAT dt);
    VOID Render(VOID);
    VOID RenderHook(VOID);
    VOID PushMS(FLOAT ms);
    VOID PushLog(LPCSTR msg, BOOL noHist=FALSE);
    LRESULT ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    VOID ClearErrorWindow();
    VOID PushErrorMessage(LPCSTR err);
    ID3DXSprite* GetTextSurface() { return mTextSurface; }

    inline VOID SetDraw2DHook(Draw2DHook hook) { *mDraw2DHook = hook; }
    inline Draw2DHook GetDraw2DHook() { return *mDraw2DHook; }
private:
    VOID DebugPanel(VOID);
    CString FormatBytes(UINT64 bytes);

#if _DEBUG
    // Error handling
    BOOL mShowError;
    CString mErrorMessage;
#endif

    ID3DXSprite* mTextSurface;
    Draw2DHook* mDraw2DHook;
};

