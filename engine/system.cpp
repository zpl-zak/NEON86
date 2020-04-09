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