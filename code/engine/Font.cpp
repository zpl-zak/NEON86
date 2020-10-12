#include "StdAfx.h"
#include "Font.h"

#include "UserInterface.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "engine.h"

CFont::CFont(LPCSTR name, INT size, INT boldness, BOOL isItalic)
{
    mFontHandle = nullptr;

    D3DXCreateFontA(
        RENDERER->GetDevice(),
        size,
        0,
        boldness,
        0,
        isItalic,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        name,
        &mFontHandle
    );
}

VOID CFont::Release()
{
    if (DelRef())
    {
        SAFE_RELEASE(mFontHandle);
    }
}

VOID CFont::RenderText(DWORD color, LPCSTR text, UINT x, UINT y, UINT w, UINT h, DWORD flags)
{
    if (!mFontHandle)
        return;

    RECT rect;
    rect.left = x;
    rect.top = y;

    if ((w == 0 || h == 0) && flags & ~FF_NOCLIP)
        CalculateRect(text, &rect);
    else
    {
        rect.right = x + w;
        rect.bottom = y + h;
    }

    RENDERER->GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
    UI->GetTextSurface()->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DONOTMODIFY_RENDERSTATE);
    mFontHandle->DrawTextA(UI->GetTextSurface(), text, -1, &rect, flags, color);
    UI->GetTextSurface()->End();
}

BOOL CFont::AddFontToDatabase(LPCSTR path)
{
    return AddFontResourceExA(FILESYSTEM->ResourcePath(path), FR_PRIVATE, nullptr) > 0;
}

VOID CFont::CalculateRect(LPCSTR text, LPRECT rect, DWORD flags)
{
    mFontHandle->DrawTextA(nullptr, text, -1, rect, flags | DT_CALCRECT, 0);
}
