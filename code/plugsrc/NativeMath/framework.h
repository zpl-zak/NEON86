#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#define NATCOLSDEF(name)\
    { #name, nativemath_##name }

#define NATCOLS(name)\
    static INT nativemath_##name(lua_State* L)
