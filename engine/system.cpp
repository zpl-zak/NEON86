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

#include <unordered_map>
static std::unordered_map<LPVOID, DWORD> gMemoryMap;

VOID neon_mempeak_update()
{
	if ((gMemUsed+gMemUsedLua) > gMemPeak)
		gMemPeak = (gMemUsed + gMemUsedLua);
}

LPVOID neon_malloc(DWORD size)
{
	gMemUsed += size;
	neon_mempeak_update();

	LPVOID mem = malloc(size);
	//gMemoryMap[mem] = size;
	return mem;
}

LPVOID neon_realloc(LPVOID mem, DWORD newSize)
{
	if (gMemoryMap.find(mem) != gMemoryMap.end())
	{	
		gMemUsed += (newSize - gMemoryMap[mem]);
		neon_mempeak_update();
		gMemoryMap.erase(mem);
	}

	LPVOID newMem = realloc(mem, newSize);
	
	//gMemoryMap[newMem] = newSize;
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

DWORD gMemUsed = 0;
DWORD gMemUsedLua = 0;
DWORD gMemPeak = 0;
DWORD gResourceCount = 0;
neon_panic_ptr* gPanicHandler = NULL;