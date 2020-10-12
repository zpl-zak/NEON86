#include <windowsx.h>
#include <strsafe.h>
#include <shobjidl.h>

#include "NeonEngine.h"
#include "Scene.h"
#include "Light.h"
#include "Font.h"
#include "ProfileManager.h"
#include "imgui.h"

#pragma comment (lib, "d3dx9.lib")

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
auto BuildWindow(HINSTANCE instance, BOOL cmdShow, LPCSTR className, LPCSTR titleName, RECT& rect) -> HWND;
auto CenterWindow(HWND hwndWindow) -> BOOL;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    HWND hWnd;
    RECT rect;
    rect.left = CW_USEDEFAULT;
    rect.top = CW_USEDEFAULT;
    rect.right = 800;
    rect.bottom = 600;

    hWnd = BuildWindow(hInstance, nCmdShow, "NeonClass", "NEON 86 | NATIVE DEMO", rect);
    CenterWindow(hWnd);

    if (!ENGINE->Init(hWnd, rect))
    {
        MessageBox(nullptr, "Failed to start engine!", "Engine load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    if (!FILESYSTEM->LoadGame(lpCmdLine))
    {
        MessageBox(nullptr, "Failed to load game!", "Game load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    // 1. Initialize sample data
    auto demoModel = new CScene();
    demoModel->LoadScene("sphere.fbx");
    D3DXMATRIX viewMat;
    D3DXMATRIX worldMat;
    auto eyePos = D3DXVECTOR3(0, 0, -5);
    auto lookPos = D3DXVECTOR3(0, 0, 0);
    auto up = D3DXVECTOR3(0, 1, 0);
    D3DXMatrixLookAtRH(&viewMat, &eyePos, &lookPos, &up);

    auto demoLight = new CLight(0);

    CFont::AddFontToDatabase("slkscr.ttf");
    auto demoFont = new CFont("Silkscreen", 20, 16, FALSE);

    D3DXMATRIX projMat;
    D3DXMatrixPerspectiveFovRH(&projMat, D3DXToRadian(62.0f), rect.right / static_cast<FLOAT>(rect.bottom), 0.01f,
                               1000.0f);

    RENDERER->SetMatrix(MATRIXKIND_PROJECTION, projMat);
    RENDERER->SetMatrix(MATRIXKIND_VIEW, viewMat);
    RENDERER->EnableLighting(TRUE);
    RENDERER->GetDevice()->SetRenderState(D3DRS_AMBIENT, 0x202069);

    MSG msg;
    auto lastTime = GetTime();

    UI->SetDraw2DHook([&]()
    {
        demoFont->RenderText(0xFFFFFFFF, "This demo is running from the native C++ side", 15, 30);
    });

    UI->SetDrawUIHook([&]()
    {
        ImGui::Begin("Hello world");
        {
            ImGui::Text("It works!");
        }
        ImGui::End();
    });

    CProfiler demoProfiler("Demo");
    ENGINE->DefaultProfiling.PushProfiler(&demoProfiler);

    while (ENGINE->IsRunning())
    {
        CProfileScope scope(&demoProfiler);
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!ENGINE->IsRunning())
            break;

        if (INPUT->GetKeyDown(VK_ESCAPE))
        {
            ENGINE->Shutdown();
            break;
        }

        auto currTime = GetTime();
        auto dt = currTime - lastTime;

        ENGINE->DefaultProfiling.UpdateProfilers(dt);

        D3DXMatrixRotationY(&worldMat, currTime);

        // 2. Render sample data
        RENDERER->BeginRender();
        {
            RENDERER->ClearBuffer(0xFF202069, CLEARFLAG_STANDARD);
            demoLight->Enable(TRUE);
            demoModel->Draw(worldMat);
            UI->Render();
        }
        RENDERER->EndRender();
        ENGINE->DefaultProfiling.IncrementFrame();

        VM->PassTime(dt);
        lastTime = GetTime();
        INPUT->Update();

        Sleep(10);
    }

    demoFont->Release();
    demoModel->Release();
    demoLight->Release();
    ENGINE->Release();

    return 0;
}

auto BuildWindow(HINSTANCE instance, BOOL cmdShow, LPCSTR className, LPCSTR titleName, RECT& rect) -> HWND
{
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = className;

    RegisterClassEx(&wc);

    const DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;

    AdjustWindowRectEx(&rect, style, FALSE, WS_EX_OVERLAPPEDWINDOW);

    const auto hWnd = CreateWindowEx(NULL,
                                     className,
                                     titleName,
                                     style,
                                     rect.left, rect.top,
                                     rect.right, rect.bottom,
                                     nullptr,
                                     nullptr,
                                     instance,
                                     nullptr);

    if (!hWnd)
    {
        const int errcode = GetLastError();
        LPVOID msg;

        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            errcode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&msg,
            0, nullptr);

        MessageBoxA(nullptr,
                    static_cast<LPCSTR>(msg),
                    "Window creation failed",
                    MB_ICONERROR);

        LocalFree(msg);
        ExitProcess(1);
    }

    ShowWindow(hWnd, SW_SHOW);

    return hWnd;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ENGINE->IsRunning() && !ENGINE->ProcessEvents(hWnd, message, wParam, lParam))
        return FALSE;

    return DefWindowProc(hWnd, message, wParam, lParam);
}

auto CenterWindow(HWND hwndWindow) -> BOOL
{
    RECT rectWindow;

    GetWindowRect(hwndWindow, &rectWindow);

    const int nWidth = rectWindow.right - rectWindow.left;
    const int nHeight = rectWindow.bottom - rectWindow.top;

    const auto nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    const auto nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    auto nX = nScreenWidth / 2 - nWidth / 2;
    int nY = nScreenHeight / 2 - nHeight / 2;

    if (nX < 0) nX = 0;
    if (nY < 0) nY = 0;
    if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
    if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

    MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

    return TRUE;
}
