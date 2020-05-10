// player.exe: A sample program loading the game data using Neon86 game engine
//

#include "stdafx.h"

#include <windowsx.h>
#include <strsafe.h>

#include "NeonEngine.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND BuildWindow(HINSTANCE instance, BOOL cmdShow, LPWSTR className, LPWSTR titleName, RECT& resolution);
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
    rect.right = 1280;
    rect.bottom = 800;

    /*if (lpCmdLine)
        SetWorkingD*/



    hWnd = BuildWindow(hInstance, nCmdShow, L"NeonClass", L"NEON 86 | PLAYER", rect);
    CenterWindow(hWnd);

    if (!ENGINE->Init(hWnd, rect))
    {
        MessageBox(NULL, L"Failed to start engine!", L"Engine load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    if (!FILESYSTEM->LoadGame(lpCmdLine))
    {
        MessageBox(NULL, L"Failed to load game!", L"Game load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    VM->Play();

    MSG msg;

    while(ENGINE->IsRunning())
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ENGINE->Think();
    }

    VM->Stop();
    ENGINE->Release();

    return 0;
}

HWND BuildWindow(HINSTANCE instance, BOOL cmdShow, LPWSTR className, LPWSTR titleName, RECT& rect)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = className;

    RegisterClassEx(&wc);

    DWORD style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;

    AdjustWindowRectEx(&rect, style, FALSE, WS_EX_OVERLAPPEDWINDOW);

    hWnd = CreateWindowEx(NULL,
        className,
        titleName,
        style,
        rect.left, rect.top,
        rect.right, rect.bottom,
        NULL,
        NULL,
        instance,
        NULL);

    RECT pica;
    GetClientRect(hWnd, &pica);

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

