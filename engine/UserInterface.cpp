#include "stdafx.h"
#include "UserInterface.h"

#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "Engine.h"
#include "Renderer.h"
#include "VM.h"

CUserInterface::CUserInterface()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(RENDERER->GetWindow());
    ImGui_ImplDX9_Init(RENDERER->GetDevice());
    ImGui_ImplWin32_EnableDpiAwareness();
}

BOOL CUserInterface::Release(VOID)
{
    ImGui_ImplDX9_Shutdown();
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

    VM->Render2D();

    ImGui::EndFrame();
    
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CUserInterface::ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

VOID CUserInterface::DebugPanel(VOID)
{
#ifdef _DEBUG
    ImGui::BeginMainMenuBar();
    {
        ImGui::Text("CPU %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Separator();
        ImGui::Text("TIME: %.2fs", VM->GetRunTime());
        ImGui::Separator();
        ImGui::Text("RESOURCES: %d MEM ENGINE: %.3f kb LUA: %.3f kb TOTAL: %.3f kb PEAK: %.3f kb", CAllocable::GetResourceCount(), gMemUsed/1024.0, gMemUsedLua / 1024.0, (gMemUsed+gMemUsedLua) / 1024.0, gMemPeak / 1024.0);
        ImGui::Separator();

        if (ImGui::Button("Restart VM"))
            VM->Restart();

        if (ImGui::Button("Pause VM"))
            VM->Pause();
    }
    ImGui::EndMainMenuBar();
    
#endif
}
