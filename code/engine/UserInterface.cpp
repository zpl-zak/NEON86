#include "stdafx.h"
#include "UserInterface.h"

#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "Engine.h"
#include "Renderer.h"
#include "VM.h"
#include "ReferenceManager.h"

CUserInterface::CUserInterface()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(RENDERER->GetWindow());
    ImGui_ImplDX9_Init(RENDERER->GetDevice());
    ImGui_ImplWin32_EnableDpiAwareness();

    ClearErrorWindow();

    D3DXCreateSprite(RENDERER->GetDevice(), &mTextSurface);
}

BOOL CUserInterface::Release(VOID)
{
    ImGui_ImplDX9_Shutdown();
    SAFE_RELEASE(mTextSurface);
    return TRUE;
}

VOID CUserInterface::Update(FLOAT dt)
{

}

VOID CUserInterface::Render(VOID)
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

LRESULT CUserInterface::ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

VOID CUserInterface::ClearErrorWindow()
{
#ifdef _DEBUG
    mShowError = FALSE;
    mErrorMessage = "";
#endif
}

VOID CUserInterface::PushErrorMessage(LPCSTR err)
{
#ifdef _DEBUG
    mShowError = TRUE;

    if (err)
        mErrorMessage += std::string(err) + "\n";
#endif
}

VOID CUserInterface::DebugPanel(VOID)
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
        ImGui::Text("MEM ENGINE: %.3f kb LUA: %.3f kb TOTAL: %.3f kb PEAK: %.3f kb", gMemUsed/1024.0, gMemUsedLua / 1024.0, ((INT64)gMemUsed+gMemUsedLua) / 1024.0, gMemPeak / 1024.0);
        ImGui::Separator();
    }
    ImGui::EndMainMenuBar();

    if (mShowError)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 150), ImGuiCond_Always);
        ImGui::Begin("Error messages", NULL, ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImGui::TextWrapped("%s", mErrorMessage.c_str());
            
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
