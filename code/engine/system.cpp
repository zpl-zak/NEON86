#include "StdAfx.h"
#include "system.h"
#include "engine.h"
#include "UserInterface.h"

auto GetTime() -> float
{
    static LARGE_INTEGER perf_freq = {0};
    static LARGE_INTEGER perf_counter = {0};
    LARGE_INTEGER counter;
    if (!perf_counter.QuadPart)
    {
        QueryPerformanceFrequency(&perf_freq);
        QueryPerformanceCounter(&perf_counter);
    }

    QueryPerformanceCounter(&counter);

    return (counter.QuadPart - perf_counter.QuadPart) / static_cast<float>(perf_freq.QuadPart);
}

auto ScaleBetween(float x, float a, float b, float na, float nb) -> float
{
    return (b - a) * (x - na) / (nb - na) + a;
}

#include <unordered_map>
static std::unordered_map<LPVOID, UINT64> gMemoryMap;

void neon_mempeak_update()
{
    if (gMemUsed + gMemUsedLua > gMemPeak)
        gMemPeak = gMemUsed + gMemUsedLua;
}

extern auto neon_malloc(size_t size) -> LPVOID
{
    gMemUsed += size;
    neon_mempeak_update();

    const auto mem = malloc(size);
    gMemoryMap[mem] = size;
    return mem;
}

extern auto neon_realloc(LPVOID mem, size_t newSize) -> LPVOID
{
    if (gMemoryMap.find(mem) != gMemoryMap.end())
    {
        gMemUsed += newSize - gMemoryMap[mem];
        neon_mempeak_update();
        gMemoryMap.erase(mem);
    }

    const auto newMem = realloc(mem, newSize);

    gMemoryMap[newMem] = newSize;
    return newMem;
}

void neon_free(LPVOID mem)
{
    if (gMemoryMap.find(mem) != gMemoryMap.end())
    {
        gMemUsed -= gMemoryMap[mem];
        gMemoryMap.erase(mem);
    }

    free(mem);
}

void HandlePanic(HWND window, LPCSTR text, LPCSTR caption, DWORD style)
{
    if (!gPanicHandler)
    {
        MessageBoxA(window, text, caption, style);
        return;
    }

    gPanicHandler(window, text, caption, style);
}

void PushLog(LPCSTR msg, bool noHist)
{
    UI->PushLog(msg, noHist);
}

UINT64 gMemUsedLua = 0;
UINT64 gMemUsed = 0;
UINT64 gMemPeak = 0;
UINT64 gResourceCount = 0;
neon_panic_ptr* gPanicHandler = nullptr;
