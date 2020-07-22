#include "stdafx.h"
#include "UserInterface.h"

#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_plot.h"

#include "Engine.h"
#include "Renderer.h"
#include "VM.h"
#include "ReferenceManager.h"
#include "ProfileManager.h"

constexpr SSIZE_T sFramerateMaxSamples = 30;

class CGraphData {
public:
    CGraphData() {
        mData.Release();

        for (UINT i=0; i<sFramerateMaxSamples; i++)
            mData.Push(0.0f);

        mMaxima = FLT_MIN;
        mMinima = FLT_MAX;
    }

    VOID Push(FLOAT ms) {
        if (VM->GetStatus() != PLAYKIND_PLAYING)
            return;

        mData.Push(ms);

        if (mData.GetCount() > sFramerateMaxSamples) {
            mData.RemoveByIndex(0);
        }
    }

    FLOAT GetMaxMS() {
        for (auto m : mData) {
            if (m > mMaxima) {
                mMaxima = m;
            }
        }

        return mMaxima;
    }

    FLOAT GetMinMS() {
        for (auto m : mData) {
            if (m < mMinima) {
                mMinima = m;
            }
        }

        return mMinima;
    }

    FLOAT* GetData() { return mData.GetData(); }
    UINT GetCount() { return mData.GetCount(); }
private:
    CArray<FLOAT> mData;
    FLOAT mMaxima, mMinima;
} sFramerateStats;

#include <sstream>

CUserInterface::CUserInterface()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
#ifdef _DEBUG
    mErrorMessage = new std::string();
#endif // _DEBUG

    mDraw2DHook = new Draw2DHook();

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(RENDERER->GetWindow());
    ImGui_ImplDX9_Init(RENDERER->GetDevice());
    ImGui_ImplWin32_EnableDpiAwareness();

    ClearErrorWindow();

    D3DXCreateSprite(RENDERER->GetDevice(), &mTextSurface);
}

BOOL CUserInterface::Release(VOID)
{
#ifdef _DEBUG
    ImGui::SaveIniSettingsToDisk(NULL);
#endif
    ImGui_ImplDX9_Shutdown();
    SAFE_RELEASE(mTextSurface);
    SAFE_DELETE(mDraw2DHook);
    
#ifdef _DEBUG
    SAFE_DELETE(mErrorMessage);
#endif // _DEBUG

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

VOID CUserInterface::RenderHook(VOID)
{
    if (*mDraw2DHook)
        (*mDraw2DHook)();
}

VOID CUserInterface::PushMS(FLOAT ms)
{
    sFramerateStats.Push(ms);
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
    *mErrorMessage = "";
#endif
}

VOID CUserInterface::PushErrorMessage(LPCSTR err)
{
#ifdef _DEBUG
    mShowError = TRUE;

    if (err)
        *mErrorMessage += std::string(err) + "\n";
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
        ImGui::Text("MEM ENGINE: %s LUA: %s TOTAL: %s PEAK: %s", FormatBytes(gMemUsed).c_str(), FormatBytes(gMemUsedLua).c_str(), FormatBytes((INT64)gMemUsed+gMemUsedLua).c_str(), FormatBytes(gMemPeak).c_str());
        ImGui::Separator();
    }
    ImGui::EndMainMenuBar();

    if (mShowError)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 150), ImGuiCond_FirstUseEver);
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

    // Profiler window
    if (ENGINE->GetRunCycleCount() > 0)
    {
        RECT res = RENDERER->GetResolution();
        ImGui::SetNextWindowPos(ImVec2(0, (FLOAT)res.bottom - 380.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Profiler", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        FLOAT profW = ImGui::GetWindowHeight();
        {
            ImGui::Columns(2, "profiler");
            ImGui::Separator();

            ImGui::Text("Profiler"); ImGui::NextColumn();
            ImGui::Text("Time"); ImGui::NextColumn();
            ImGui::Separator();

            for (INT i = 0; i < MAX_NEON_PROFILERS; i++)
            {
                CProfiler* profiler = ENGINE->GetProfilers()[i];
                ImGui::Text("%s Time", profiler->GetName().Str()); ImGui::NextColumn();
                ImGui::Text("%f ms", profiler->GetDelta()); ImGui::NextColumn();
            }

            ImGui::Separator();

            ImGui::Text("Other Time"); ImGui::NextColumn();
            ImGui::Text("%f ms", ((DOUBLE)ENGINE->GetTotalRunTime() - ENGINE->GetTotalMeasuredRunTime())); ImGui::NextColumn();
            
            ImGui::Text("Total Time"); ImGui::NextColumn();
            ImGui::Text("%f ms (%.02f fps)", ENGINE->GetTotalRunTime(), (1000.0f / ENGINE->GetTotalRunTime())); ImGui::NextColumn();

            ImGui::Columns(1);
            ImGui::Separator();
            {
                static UINT32 selection_start = 0, selection_length = 0;
                
                ImGui::PlotConfig conf;
                conf.values.count = sFramerateMaxSamples;
                conf.values.ys = sFramerateStats.GetData();
                conf.values.offset = 0;
                conf.values.color = ImColor(0, 0, 0);
                conf.scale.min = sFramerateStats.GetMinMS();
                conf.scale.max = sFramerateStats.GetMaxMS();
                conf.scale.type = ImGui::PlotConfig::Scale::Linear;
                conf.tooltip.show = true;
                conf.grid_x.show = true;
                conf.grid_x.size = 10.0f;
                conf.grid_x.subticks = 5;
                conf.grid_y.show = true;
                conf.grid_y.size = 10.0f;
                conf.grid_y.subticks = 5;
                conf.selection.show = true;
                conf.selection.start = &selection_start;
                conf.selection.length = &selection_length;
                conf.frame_size = ImVec2(sFramerateMaxSamples*10, 200);
                conf.line_thickness = 4.0f;
                conf.overlay_text = "Framerate / Second";
                ImGui::Plot("frameratePlot", conf);
            }
        }
        ImGui::End();
    }
#endif
}

std::string CUserInterface::FormatBytes(UINT64 bytes)
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
