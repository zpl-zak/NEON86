#pragma once

#include "system.h"

#define UI CEngine::the()->GetUI()

#include <functional>

using Draw2DHook = std::function<void()>;
using DrawUIHook = std::function<void()>;

class ENGINE_API CUserInterface
{
public:
    CUserInterface();
    auto Release(VOID) -> BOOL;
    VOID Update(FLOAT dt);
    VOID Render(VOID);
    VOID RenderHook(VOID) const;
    VOID PushMS(FLOAT ms);
    VOID PushLog(LPCSTR msg, BOOL noHist = FALSE);
    static auto ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT;

    VOID ClearErrorWindow();
    VOID PushErrorMessage(LPCSTR err);
    auto GetTextSurface() const -> ID3DXSprite* { return mTextSurface; }

    VOID SetDraw2DHook(Draw2DHook hook) const { *mDraw2DHook = hook; }
    auto GetDraw2DHook() const -> Draw2DHook { return *mDraw2DHook; }

    VOID SetDrawUIHook(DrawUIHook hook) const { *mDrawUIHook = hook; }
    auto GetDrawUIHook() const -> DrawUIHook { return *mDrawUIHook; }
private:
    VOID DebugPanel(VOID) const;
    static auto FormatBytes(UINT64 bytes) -> CString;
    VOID SetupRender2D();

    #if _DEBUG
    // Error handling
    BOOL mShowError;
    CString mErrorMessage;
    #endif

    ID3DXSprite* mTextSurface;
    Draw2DHook* mDraw2DHook;
    DrawUIHook* mDrawUIHook;
};
