// player.exe: A sample program showcasing native game support
//

#include <windowsx.h>
#include <strsafe.h>
#include <shobjidl.h>

#include "NeonEngine.h"
#include "Scene.h"
#include "Light.h"
#include "Font.h"

#pragma comment (lib, "d3dx9.lib")

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND BuildWindow(HINSTANCE instance, BOOL cmdShow, LPCSTR className, LPCSTR titleName, RECT& rect);
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
    rect.right = 800;
    rect.bottom = 600;

    hWnd = BuildWindow(hInstance, nCmdShow, "NeonClass", "NEON 86 | NATIVE DEMO", rect);
    CenterWindow(hWnd);

    if (!ENGINE->Init(hWnd, rect))
    {
        MessageBox(NULL, "Failed to start engine!", "Engine load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    if (!FILESYSTEM->LoadGame(lpCmdLine))
    {
        MessageBox(NULL, "Failed to load game!", "Game load failure", MB_OK);
        ENGINE->Release();
        return 0;
    }

    // 1. Initialize sample data
    CScene* demoModel = new CScene();
    demoModel->LoadScene("sphere.fbx");
    D3DXMATRIX viewMat;
    D3DXMATRIX worldMat;
    D3DXVECTOR3 eyePos = D3DXVECTOR3(0,0,-5);
    D3DXVECTOR3 lookPos = D3DXVECTOR3(0,0,0);
    D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);
    D3DXMatrixLookAtRH(&viewMat, &eyePos, &lookPos, &up);

    CLight* demoLight = new CLight(0);

    CFont::AddFontToDatabase("slkscr.ttf");
    CFont* demoFont = new CFont("Silkscreen", 20, 16, FALSE);

    D3DXMATRIX projMat;
    D3DXMatrixPerspectiveFovRH(&projMat, D3DXToRadian(62.0f), rect.right / (FLOAT)rect.bottom, 0.01f, 1000.0f);

    RENDERER->SetMatrix(MATRIXKIND_PROJECTION, projMat);
    RENDERER->SetMatrix(MATRIXKIND_VIEW, viewMat);
    RENDERER->EnableLighting(TRUE);
    RENDERER->GetDevice()->SetRenderState(D3DRS_AMBIENT, 0x202069);

    MSG msg;
    FLOAT lastTime = GetTime();

    UI->SetDraw2DHook([&]() {
        demoFont->RenderText(0xFFFFFFFF, "This demo is running from the native C++ side", 15, 30);
    });

    while (ENGINE->IsRunning())
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!ENGINE->IsRunning())
            break;

        if (INPUT->GetKeyDown(VK_ESCAPE)) {
            ENGINE->Shutdown();
            break;
        }

        FLOAT currTime = GetTime();
        FLOAT dt = currTime - lastTime;

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

HWND BuildWindow(HINSTANCE instance, BOOL cmdShow, LPCSTR className, LPCSTR titleName, RECT& rect)
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

    int nX = nScreenWidth / 2 - nWidth / 2;
    int nY = nScreenHeight / 2 - nHeight / 2;

    if (nX < 0) nX = 0;
    if (nY < 0) nY = 0;
    if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
    if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

    MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

    return TRUE;
}
