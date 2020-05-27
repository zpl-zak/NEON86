#include "StdAfx.h"
#include "Font.h"

#include "UserInterface.h"
#include "Renderer.h"
#include "VM.h"
#include "FileSystem.h"
#include "engine.h"

CFont::CFont(LPCSTR name, INT size, INT boldness, BOOL isItalic)
{
    mFontHandle = NULL;

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
        DEFAULT_PITCH|FF_DONTCARE,
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

VOID CFont::RenderText(DWORD color, LPCSTR text, UINT x, UINT y, UINT w, UINT h)
{
    if (!mFontHandle)
        return;

    RECT rect;
    rect.left = x;
    rect.top = y;
    
    if (w == 0 || h == 0)
        CalculateRect(text, &rect);
    else
    {
        rect.right = x + w;
        rect.bottom = y + h;
    }

    mFontHandle->DrawTextA(UI->GetTextSurface(), text, -1, &rect, DT_LEFT|DT_WORDBREAK, color);
}

BOOL CFont::AddFontToDatabase(LPCSTR path)
{
    return AddFontResourceExA(FILESYSTEM->ResourcePath(RESOURCEKIND_USER, path), FR_PRIVATE, 0) > 0;
}

VOID CFont::CalculateRect(LPCSTR text, LPRECT rect)
{
    mFontHandle->DrawTextA(NULL, text, -1, rect, DT_LEFT|DT_CALCRECT|DT_WORDBREAK, 0);
}
