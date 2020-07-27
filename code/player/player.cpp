// player.exe: A sample program loading the game data using Neon86 game engine
//

#include "stdafx.h"

#include <windowsx.h>
#include <strsafe.h>
#include <shobjidl.h>

#include "NeonEngine.h"
#include "imgui.h"

#ifdef ENABLE_BORDERLESS
#define DEV_BORDERLESS
#endif

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND BuildWindow(HINSTANCE instance, BOOL borderless, LPCSTR className, LPCSTR titleName, RECT& resolution);
BOOL CenterWindow(HWND hwndWindow);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
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
        MessageBox(NULL, "Failed to start engine!", "Engine load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    ENGINE->DefaultProfiling.SetupDefaultProfilers();

    if (!FILESYSTEM->LoadGame(lpCmdLine))
    {
        MessageBox(NULL, "Failed to load game!", "Game load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    VM->Play();
    ENGINE->Run();

    return 0;
}

HWND BuildWindow(HINSTANCE instance, BOOL borderless, LPCSTR className, LPCSTR titleName, RECT& resolution)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = className;

    RegisterClassEx(&wc);

#ifndef DEV_BORDERLESS
    DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
    AdjustWindowRectEx(&resolution, style, FALSE, WS_EX_OVERLAPPEDWINDOW);
#else
    DWORD style = WS_VISIBLE | WS_POPUP;
#endif

    hWnd = CreateWindowEx(NULL,
        className,
        titleName,
        style,
        resolution.left, resolution.top,
        resolution.right, resolution.bottom,
        NULL,
        NULL,
        instance,
        NULL);

    if (!hWnd)
    {
        int errcode = GetLastError();
        LPVOID msg;

        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errcode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&msg,
            0, NULL);
        
        MessageBoxA(NULL,
            (LPCSTR)msg,
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

BOOL CenterWindow(HWND hwndWindow)
{
    RECT rectWindow;

    GetWindowRect(hwndWindow, &rectWindow);

    int nWidth = rectWindow.right - rectWindow.left;
    int nHeight = rectWindow.bottom - rectWindow.top;

    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    int nX = nScreenWidth / 2 - nWidth/2;
    int nY = nScreenHeight / 2 - nHeight/2;

    if (nX < 0) nX = 0;
    if (nY < 0) nY = 0;
    if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
    if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

    MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

    return TRUE;
}
