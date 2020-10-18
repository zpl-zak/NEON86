#include "stdafx.h"

#include <windowsx.h>
#include <strsafe.h>
#include <shobjidl.h>

#include "NeonEngine.h"

#ifdef ENABLE_BORDERLESS
#define DEV_BORDERLESS
#endif

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
auto BuildWindow(HINSTANCE instance, BOOL borderless, LPCSTR className, LPCSTR titleName, RECT& resolution) -> HWND;
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
    #ifndef DEV_BORDERLESS
    rect.right = 1600;
    rect.bottom = 900;
    hWnd = BuildWindow(hInstance, FALSE, "NeonClass", "NEON 86 | PLAYER", rect);
    #else
    rect.right = GetSystemMetrics(SM_CXSCREEN);
    rect.bottom = GetSystemMetrics(SM_CYSCREEN);
    hWnd = BuildWindow(hInstance, TRUE, "NeonClass", "NEON 86 | PLAYER", rect);
    #endif

    CenterWindow(hWnd);

    if (!ENGINE->Init(hWnd, rect))
    {
        MessageBox(nullptr, "Failed to start engine!", "Engine load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    ENGINE->DefaultProfiling.SetupDefaultProfilers();

    if (!FILESYSTEM->LoadGame(lpCmdLine))
    {
        MessageBox(nullptr, "Failed to load game!", "Game load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    VM->Play();
    ENGINE->Run();

    return 0;
}

auto BuildWindow(HINSTANCE instance, BOOL borderless, LPCSTR className, LPCSTR titleName, RECT& resolution) -> HWND
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = className;

    RegisterClassEx(&wc);

    #ifndef DEV_BORDERLESS
    DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
    AdjustWindowRectEx(&resolution, style, FALSE, WS_EX_OVERLAPPEDWINDOW);
    #else
    const auto style = WS_VISIBLE | WS_POPUP;
    #endif

    hWnd = CreateWindowEx(NULL,
                          className,
                          titleName,
                          style,
                          resolution.left, resolution.top,
                          resolution.right, resolution.bottom,
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
    auto nY = nScreenHeight / 2 - nHeight / 2;

    if (nX < 0) nX = 0;
    if (nY < 0) nY = 0;
    if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
    if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

    MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

    return TRUE;
}
