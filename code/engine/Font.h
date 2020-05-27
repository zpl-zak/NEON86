#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class CFont : public CAllocable<CFont>, public CReferenceCounter
{
public:
    CFont(LPCSTR path, INT size, INT boldness, BOOL isItalic);
    VOID Release();

    VOID RenderText(DWORD color, LPCSTR text, UINT x, UINT y, UINT w = 0, UINT h = 0);

    static BOOL AddFontToDatabase(LPCSTR path);
private:
    ID3DXFont* mFontHandle;

    VOID CalculateRect(LPCSTR text, LPRECT rect);
};