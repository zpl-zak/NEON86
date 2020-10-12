#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class ENGINE_API CFont : public CAllocable<CFont>, public CReferenceCounter
{
public:
    CFont(LPCSTR path, int size, int boldness, bool isItalic);
    void Release();

    void RenderText(DWORD color, LPCSTR text, unsigned int x, unsigned int y, unsigned int w = 0, unsigned int h = 0, DWORD flags = 0);
    void CalculateRect(LPCSTR text, LPRECT rect, DWORD flags = 0);

    static auto AddFontToDatabase(LPCSTR path) -> bool;
private:
    ID3DXFont* mFontHandle;
};
