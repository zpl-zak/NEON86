#include "StdAfx.h"
#include "system.h"

FLOAT GetTime(BOOL flush)
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

    if (flush)
    {
        QueryPerformanceCounter(&perf_counter);
        counter.QuadPart = perf_counter.QuadPart;
    }

	return (counter.QuadPart - perf_counter.QuadPart) / (FLOAT)(perf_freq.QuadPart);
}