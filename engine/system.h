#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#define SAFE_DELETE(memory) if(memory) { delete memory; memory = NULL; }
#define SAFE_DELETE_ARRAY(memory) if(memory) { delete[] memory; memory = NULL; }
#define SAFE_FREE(memory) if(memory) { neon_free(memory); memory = NULL; }
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = NULL; } }

extern FLOAT GetTime(BOOL flush = FALSE);


extern DWORD gMemUsed, gMemUsedLua, gMemPeak;
extern VOID neon_mempeak_update();
extern LPVOID neon_malloc(DWORD size);
extern LPVOID neon_realloc(LPVOID mem, DWORD newSize);
extern VOID neon_free(LPVOID mem);

/// panic handling
#define NEON_PANIC_FN(name) VOID name(HWND window, LPCSTR text, LPCSTR caption, DWORD style);
typedef NEON_PANIC_FN(neon_panic_ptr);
extern neon_panic_ptr* gPanicHandler;
extern void HandlePanic(HWND window, LPCSTR text, LPCSTR caption, DWORD style);

/// zpl
#include "zpl_macros.h"

/// lua
#include "lua_macros.h"
