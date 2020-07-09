#include "StdAfx.h"
#include "system.h"

FLOAT GetTime()
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

	return (counter.QuadPart - perf_counter.QuadPart) / (FLOAT)(perf_freq.QuadPart);
}

FLOAT ScaleBetween(FLOAT x, FLOAT a, FLOAT b, FLOAT na, FLOAT nb)
{
	return (b - a) * (x - na) / (nb - na) + a;
}

#include <unordered_map>
static std::unordered_map<LPVOID, size_t> gMemoryMap;

VOID neon_mempeak_update()
{
	if ((gMemUsed+gMemUsedLua) > gMemPeak)
		gMemPeak = (gMemUsed + gMemUsedLua);
}

extern LPVOID neon_malloc(size_t size)
{
	gMemUsed += size;
	neon_mempeak_update();

	LPVOID mem = malloc(size);
	gMemoryMap[mem] = size;
	return mem;
}

extern LPVOID neon_realloc(LPVOID mem, size_t newSize)
{
	if (gMemoryMap.find(mem) != gMemoryMap.end())
	{	
		gMemUsed += (newSize - gMemoryMap[mem]);
		neon_mempeak_update();
		gMemoryMap.erase(mem);
	}

	LPVOID newMem = realloc(mem, newSize);
	
	gMemoryMap[newMem] = newSize;
	return newMem;
}

VOID neon_free(LPVOID mem)
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

size_t gMemUsed = 0;
size_t gMemUsedLua = 0;
size_t gMemPeak = 0;
size_t gResourceCount = 0;
neon_panic_ptr* gPanicHandler = NULL;