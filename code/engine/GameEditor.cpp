#include "stdafx.h"
#include "GameEditor.h"

#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "Engine.h"
#include "Renderer.h"
#include "VM.h"
#include "ReferenceManager.h"

#include <sstream>

CGameEditor::CGameEditor()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    #ifdef _DEBUG
mErrorMessage = new std::string();
#endif // _DEBUG

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(RENDERER->GetWindow());
    ImGui_ImplDX9_Init(RENDERER->GetDevice());
    ImGui_ImplWin32_EnableDpiAwareness();

    ClearErrorWindow();

    D3DXCreateSprite(RENDERER->GetDevice(), &mTextSurface);
}

BOOL CGameEditor::Release(VOID)
{
    ImGui_ImplDX9_Shutdown();
    SAFE_RELEASE(mTextSurface);
    #ifdef _DEBUG
SAFE_DELETE(mErrorMessage);
#endif // _DEBUG
    return TRUE;
}

VOID CGameEditor::Update(FLOAT dt)
{

}

VOID CGameEditor::Render(VOID)
{
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    DebugPanel();

    ImGui::EndFrame();
    
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CGameEditor::ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

VOID CGameEditor::ClearErrorWindow()
{
#ifdef _DEBUG
    mShowError = FALSE;
    *mErrorMessage = "";
#endif
}

VOID CGameEditor::PushErrorMessage(LPCSTR err)
{
#ifdef _DEBUG
    mShowError = TRUE;

    if (err)
        *mErrorMessage += std::string(err) + "\n";
#endif
}

VOID CGameEditor::DebugPanel(VOID)
{
#ifdef _DEBUG
    ImGui::BeginMainMenuBar();
    {
        if (ImGui::Button("Restart VM"))
            VM->Restart();

        if (ImGui::Button("Pause VM"))
            VM->Pause();

        ImGui::Text("CPU %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Separator();
        ImGui::Text("TIME: %.2fs", VM->GetRunTime());
        ImGui::Separator();
        ImGui::Text("RESOURCES: %d", gResourceCount);
        ImGui::Separator();
        ImGui::Text("MEM ENGINE: %s LUA: %s TOTAL: %s PEAK: %s", FormatBytes(gMemUsed).c_str(), FormatBytes(gMemUsedLua).c_str(), FormatBytes((INT64)gMemUsed+gMemUsedLua).c_str(), FormatBytes(gMemPeak).c_str());
        ImGui::Separator();
    }
    ImGui::EndMainMenuBar();

    if (mShowError)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 150), ImGuiCond_Always);
        ImGui::Begin("Error messages", NULL, ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImGui::TextWrapped("%s", mErrorMessage->c_str());
            
            if (ImGui::Button("Restart VM"))
                VM->Restart();

            ImGui::SameLine();

            if (ImGui::Button("Exit Game"))
                ENGINE->Shutdown();
        }
        ImGui::End();
    }
#endif
}

std::string CGameEditor::FormatBytes(UINT64 bytes)
{
    const std::string suffixes[] = { "B", "KB", "MB", "GB", "TB" };
    BYTE suffixId;
    DOUBLE formattedBytes = 0.0;

    for (suffixId = 0; suffixId < 5 && bytes >= 1024; suffixId++, bytes /= 1024)
    {
        formattedBytes = bytes / 1024.0;
    }

    std::stringstream ss;
    ss.precision(2);
    ss << std::fixed << formattedBytes << " " << suffixes[suffixId];

    return ss.str();
}
