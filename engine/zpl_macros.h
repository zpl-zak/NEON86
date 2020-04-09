#pragma once

#if defined(ZPL_CONCAT_EX)
#  undef ZPL_CONCAT_EX
#endif
#define ZPL_CONCAT_EX(a,b) a##b

#if defined(ZPL_CONCAT)
#  undef ZPL_CONCAT
#endif
#define ZPL_CONCAT(a,b) ZPL_CONCAT_EX(a,b)

#ifndef ZPL_JOIN_MACROS
#define ZPL_JOIN_MACROS

#define ZPL_JOIN2 ZPL_CONCAT
#define ZPL_JOIN3(a, b, c) ZPL_JOIN2(ZPL_JOIN2(a, b), c)
#define ZPL_JOIN4(a, b, c, d) ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(a, b), c), d)
#define ZPL_JOIN5(a, b, c, d, e) ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(a, b), c), d), e)
#define ZPL_JOIN6(a, b, c, d, e, f) ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(a, b), c), d), e), f)
#endif