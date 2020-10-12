#include "StdAfx.h"
#include "Font.h"

#include "UserInterface.h"
#include "Renderer.h"
#include "FileSystem.h"
#include "engine.h"

CFont::CFont(LPCSTR name, int size, int boldness, bool isItalic)
{
    mFontHandle = nullptr;

    D3DXCreateFontA(
        RENDERER->GetDevice(),
        size,
        0,
        boldness,
        0,
        static_cast<BOOL>(isItalic),
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        name,
        &mFontHandle
    );
}

void CFont::Release()
{
    if (DelRef())
    {
        SAFE_RELEASE(mFontHandle);
    }
}

void CFont::RenderText(DWORD color, LPCSTR text, unsigned int x, unsigned int y, unsigned int w, unsigned int h,
                       DWORD flags)
{
    if (mFontHandle == nullptr)
    {
        return;
    }

    RECT rect;
    rect.left = x;
    rect.top = y;

    if ((w == 0 || h == 0) && ((flags & ~FF_NOCLIP) != 0u))
    {
        CalculateRect(text, &rect);
    }
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

auto CFont::AddFontToDatabase(LPCSTR path) -> bool
{
    return AddFontResourceExA(FILESYSTEM->ResourcePath(path), FR_PRIVATE, nullptr) > 0;
}

void CFont::CalculateRect(LPCSTR text, LPRECT rect, DWORD flags)
{
    mFontHandle->DrawTextA(nullptr, text, -1, rect, flags | DT_CALCRECT, 0);
}
