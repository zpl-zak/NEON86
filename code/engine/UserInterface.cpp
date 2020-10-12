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

class CGraphData
{
public:
    CGraphData()
    {
        mMaxima = FLT_MIN;
        mMinima = FLT_MAX;

        mSelectionStart = 0;
        mSelectionLength = 0;
        mPaused = FALSE;
    }

    VOID Push(FLOAT ms)
    {
        if (VM->GetStatus() != PLAYKIND_PLAYING)
            return;

        if (mPaused)
            return;

        mData.Push(ms);

        if (mData.GetCount() > sFramerateMaxSamples)
        {
            mData.RemoveByIndex(0);

            if (mSelectionStart > 0)
            {
                mSelectionStart--;
            }
        }
    }

    auto GetMaxMS() -> FLOAT
    {
        for (auto m : mData)
        {
            if (m > mMaxima)
            {
                mMaxima = m;
            }
        }

        return mMaxima;
    }

    auto GetMinMS() -> FLOAT
    {
        for (auto m : mData)
        {
            if (m < mMinima)
            {
                mMinima = m;
            }
        }

        return mMinima;
    }

    auto GetData() const -> FLOAT* { return mData.GetData(); }
    auto GetCount() const -> UINT { return mData.GetCount(); }

    VOID Render()
    {
        const auto res = RENDERER->GetResolution();
        ImGui::SetNextWindowSize({316, 440}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(0, static_cast<FLOAT>(res.bottom) - 440), ImGuiCond_FirstUseEver);
        ImGui::Begin("Profiler", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImGui::Columns(2, "profiler");
            ImGui::Separator();

            ImGui::Text("Profiler");
            ImGui::NextColumn();
            ImGui::Text("Time");
            ImGui::NextColumn();

            if (ENGINE->DefaultProfiling.GetRunCycleCount() > 0 && ENGINE->DefaultProfiling.GetProfilers().GetCount() >
                0)
            {
                ImGui::Separator();

                for (UINT i = 0; i < ENGINE->DefaultProfiling.GetProfilers().GetCount(); i++)
                {
                    const auto profiler = ENGINE->DefaultProfiling.GetProfilers()[i];
                    ImGui::Text("%s Time", profiler->GetName().Str());
                    ImGui::NextColumn();
                    ImGui::Text("%f ms", profiler->GetDelta());
                    ImGui::NextColumn();
                }
            }

            ImGui::Separator();

            ImGui::Text("Other Time");
            ImGui::NextColumn();
            ImGui::Text(
                "%f ms",
                static_cast<DOUBLE>(ENGINE->DefaultProfiling.GetTotalRunTime()) - ENGINE->DefaultProfiling.
                                                                                          GetTotalMeasuredRunTime());
            ImGui::NextColumn();

            ImGui::Text("Total Time");
            ImGui::NextColumn();
            ImGui::Text("%f ms (%.02f fps)", ENGINE->DefaultProfiling.GetTotalRunTime(),
                        1000.0f / ENGINE->DefaultProfiling.GetTotalRunTime());
            ImGui::NextColumn();

            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::Checkbox("Is paused", &mPaused);
            {
                ImGui::PlotConfig conf;
                conf.values.count = static_cast<int>(fmin(sFramerateMaxSamples, mData.GetCount()));
                conf.values.ys = GetData();
                conf.values.offset = 0;
                conf.values.color = ImColor(0, 0, 0);
                conf.scale.min = GetMinMS();
                conf.scale.max = GetMaxMS();
                conf.scale.type = ImGui::PlotConfig::Scale::Linear;
                conf.tooltip.show = true;
                conf.tooltip.format = "%.0s%.02f ms";
                conf.grid_x.show = true;
                conf.grid_x.size = 10.0f;
                conf.grid_x.subticks = 5;
                conf.grid_y.show = true;
                conf.grid_y.size = 10.0f;
                conf.grid_y.subticks = 5;
                conf.selection.show = true;
                conf.selection.start = &mSelectionStart;
                conf.selection.length = &mSelectionLength;
                conf.frame_size = ImVec2(sFramerateMaxSamples * 10, 200);
                conf.line_thickness = 4.0f;
                conf.overlay_text = "Total Time (ms)";
                Plot("frameratePlot", conf);
            }
            ImGui::Separator();

            // avg ms by selection
            if (mSelectionLength > 0)
            {
                auto avgMs = 0.0f;

                for (auto i = mSelectionStart; i < mSelectionStart + mSelectionLength && i < mData.GetCount(); i++)
                    avgMs += mData[i];

                avgMs /= mSelectionLength;

                ImGui::Text("Average Time (selection): %f ms", avgMs);
            }
            else
            {
                auto avgMs = 0.0f;

                for (UINT i = 0; i < mData.GetCount(); i++)
                    avgMs += mData[i];

                avgMs /= mData.GetCount();

                ImGui::Text("Average Time: %f ms", avgMs);
            }

            ImGui::Text("Min Time: %f ms", GetMinMS());
            ImGui::Text("Max Time: %f ms", GetMaxMS());
        }
        ImGui::End();
    }

private:
    CArray<FLOAT> mData;
    FLOAT mMaxima, mMinima;
    UINT32 mSelectionStart;
    UINT32 mSelectionLength;
    bool mPaused;
} sFramerateStats;

class CLogWindow
{
public:
    CLogWindow()
    {
        Clear();
        mAutoScroll = TRUE;
        mPaused = FALSE;
    }

    VOID Clear()
    {
        mBuffer.clear();
        mLineOffsets.clear();
        mLineOffsets.push_back(0);
    }

    VOID Push(LPCSTR msg)
    {
        if (mPaused)
            return;

        auto oldSize = mBuffer.size();
        mBuffer.append(msg);
        for (const auto newSize = mBuffer.size(); oldSize < newSize; oldSize++)
        {
            if (mBuffer[oldSize] == '\n')
                mLineOffsets.push_back(oldSize + 1);
        }
    }

    VOID Pause()
    {
        mPaused = !mPaused;
    }

    VOID Render()
    {
        const auto res = RENDERER->GetResolution();

        ImGui::SetNextWindowSizeConstraints({220, 300}, {
                                                static_cast<FLOAT>(res.right), static_cast<FLOAT>(res.bottom)
                                            });
        ImGui::SetNextWindowSize({576, 408}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(static_cast<FLOAT>(res.right) - 576, static_cast<FLOAT>(res.bottom) - 408),
                                ImGuiCond_FirstUseEver);
        ImGui::Begin("Output", nullptr);
        {
            if (ImGui::BeginPopup("Options"))
            {
                ImGui::Checkbox("Auto-scroll", &mAutoScroll);
                ImGui::EndPopup();
            }

            if (ImGui::Button("Options"))
                ImGui::OpenPopup("Options");
            ImGui::SameLine();
            if (ImGui::Button("Clear"))
            {
                Clear();
            }
            ImGui::SameLine();
            if (ImGui::Button("Pause"))
            {
                Pause();
            }
            ImGui::SameLine();
            if (ImGui::Button("Copy to clipboard"))
            {
                ImGui::LogToClipboard();
            }
            if (mPaused)
            {
                ImGui::SameLine();
                ImGui::Text("(paused)");
            }

            ImGui::BeginChildFrame(0xDEADC0DE, ImVec2(0, 0), ImGuiWindowFlags_HorizontalScrollbar);
            {
                ImGuiListClipper clipper;
                const auto buf = mBuffer.begin();
                const auto bufEnd = mBuffer.end();

                clipper.Begin(mLineOffsets.Size);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                while (clipper.Step())
                {
                    for (auto line = clipper.DisplayStart; line < clipper.DisplayEnd; line++)
                    {
                        const auto start = buf + mLineOffsets[line];
                        const auto end = line + 1 < mLineOffsets.Size ? buf + mLineOffsets[line + 1] - 1 : bufEnd;
                        ImGui::TextUnformatted(start, end);
                    }
                }
                clipper.End();
                ImGui::PopStyleVar();

                if (mAutoScroll && ImGui::GetScrollY() + 20 >= ImGui::GetScrollMaxY())
                    ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChildFrame();
        }
        ImGui::End();
    }

private:
    ImGuiTextBuffer mBuffer;
    ImVector<INT> mLineOffsets;
    bool mAutoScroll;
    bool mPaused;
} sLogWindow;

#include <sstream>

CUserInterface::CUserInterface()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    (void)io;
    #ifdef _DEBUG
    mErrorMessage = "";
    #endif // _DEBUG

    mDraw2DHook = new Draw2DHook();
    mDrawUIHook = new DrawUIHook();

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(RENDERER->GetWindow());
    ImGui_ImplDX9_Init(RENDERER->GetDevice());
    ImGui_ImplWin32_EnableDpiAwareness();

    ClearErrorWindow();

    D3DXCreateSprite(RENDERER->GetDevice(), &mTextSurface);
}

auto CUserInterface::Release(VOID) -> BOOL
{
    #ifdef _DEBUG
    ImGui::SaveIniSettingsToDisk("imgui.ini");
    #endif
    ImGui_ImplDX9_Shutdown();
    SAFE_RELEASE(mTextSurface);
    SAFE_DELETE(mDraw2DHook);
    SAFE_DELETE(mDrawUIHook);

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

    SetupRender2D();

    if (*mDrawUIHook)
        (*mDrawUIHook)();

    DebugPanel();

    ImGui::EndFrame();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

VOID CUserInterface::RenderHook(VOID) const
{
    if (*mDraw2DHook)
        (*mDraw2DHook)();
}

VOID CUserInterface::PushMS(FLOAT ms)
{
    sFramerateStats.Push(ms);
}

VOID CUserInterface::PushLog(LPCSTR msg, BOOL noHist)
{
    OutputDebugStringA(msg);

    #ifdef _DEBUG
    if (!noHist)
        sLogWindow.Push(msg);
    #endif
}

extern IMGUI_IMPL_API auto ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT;

auto CUserInterface::ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

VOID CUserInterface::ClearErrorWindow()
{
    #ifdef _DEBUG
    mShowError = FALSE;
    mErrorMessage = "\0";
    #endif
}

VOID CUserInterface::PushErrorMessage(LPCSTR err)
{
    #ifdef _DEBUG
    mShowError = TRUE;

    if (err)
        mErrorMessage = CString::Format("%s %s\n", mErrorMessage.Str(), err);
    #endif
}

VOID CUserInterface::DebugPanel(VOID) const
{
    #ifdef _DEBUG
    ImGui::BeginMainMenuBar();
    {
        if (ImGui::Button(" X "))
            ENGINE->Shutdown();

        if (ImGui::Button("Restart VM"))
            VM->Restart();

        if (ImGui::Button("Pause VM"))
            VM->Pause();

        ImGui::Text("RESOURCES: %d", gResourceCount);
        ImGui::Separator();
        ImGui::Text("MEM ENGINE: %s", FormatBytes(gMemUsed).Str());
        ImGui::Separator();
        ImGui::Text("LUA: %s", FormatBytes(gMemUsedLua).Str());
        ImGui::Separator();
        ImGui::Text("TOTAL: %s", FormatBytes(static_cast<INT64>(gMemUsed) + gMemUsedLua).Str());
        ImGui::Separator();
        ImGui::Text("PEAK: %s", FormatBytes(gMemPeak).Str());
        ImGui::Separator();
        ImGui::Text("CPU %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Separator();
        ImGui::Text("TIME: %.2fs", VM->GetRunTime());
        ImGui::Separator();
    }
    ImGui::EndMainMenuBar();

    if (mShowError)
    {
        ImGui::SetNextWindowSize(ImVec2(500, 150), ImGuiCond_FirstUseEver);
        ImGui::Begin("Error messages", nullptr,
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_AlwaysAutoResize);
        {
            ImGui::TextWrapped("%s", mErrorMessage.Str());

            if (ImGui::Button("Restart VM"))
                VM->Restart();

            ImGui::SameLine();

            if (ImGui::Button("Exit Game"))
                ENGINE->Shutdown();
        }
        ImGui::End();
    }

    sFramerateStats.Render();
    sLogWindow.Render();
    #endif
}

auto CUserInterface::FormatBytes(UINT64 bytes) -> CString
{
    const std::string suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    BYTE suffixId;
    auto formattedBytes = 0.0;

    for (suffixId = 0; suffixId < 5 && bytes >= 1024; suffixId++, bytes /= 1024)
    {
        formattedBytes = bytes / 1024.0;
    }

    std::stringstream ss;
    ss.precision(2);
    ss << std::fixed << formattedBytes << " " << suffixes[suffixId];

    return ss.str().c_str();
}

VOID CUserInterface::SetupRender2D()
{
    auto dev = RENDERER->GetDevice();
    dev->SetPixelShader(nullptr);
    dev->SetVertexShader(nullptr);
    dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    dev->SetRenderState(D3DRS_LIGHTING, FALSE);
    dev->SetRenderState(D3DRS_ZENABLE, FALSE);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    dev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    dev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
    dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    // NEON86: Render 2D
    IDirect3DStateBlock9* neonsbt = nullptr;
    RENDERER->GetDevice()->CreateStateBlock(D3DSBT_ALL, &neonsbt);
    neonsbt->Capture();
    {
        CProfileScope scope(ENGINE->DefaultProfiling.INTERNAL_GetRender2DProfiler());
        UI->RenderHook();
        VM->Render2D();
    }

    neonsbt->Apply();
    neonsbt->Release();
}
