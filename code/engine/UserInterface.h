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
    auto Release(void) -> bool;
    void Update(float dt);
    void Render(void);
    void RenderHook(void) const;
    void PushMS(float ms);
    void PushLog(LPCSTR msg, bool noHist = FALSE);
    static auto ProcessEvents(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam) -> LRESULT;

    void ClearErrorWindow();
    void PushErrorMessage(LPCSTR err);
    auto GetTextSurface() const -> ID3DXSprite* { return mTextSurface; }

    void SetDraw2DHook(Draw2DHook hook) const { *mDraw2DHook = hook; }
    auto GetDraw2DHook() const -> Draw2DHook { return *mDraw2DHook; }

    void SetDrawUIHook(DrawUIHook hook) const { *mDrawUIHook = hook; }
    auto GetDrawUIHook() const -> DrawUIHook { return *mDrawUIHook; }
private:
    void DebugPanel(void) const;
    static auto FormatBytes(UINT64 bytes) -> CString;
    void SetupRender2D();

    #if _DEBUG
    // Error handling
    bool mShowError;
    CString mErrorMessage;
    #endif

    ID3DXSprite* mTextSurface;
    Draw2DHook* mDraw2DHook;
    DrawUIHook* mDrawUIHook;
};
