#pragma once

#include <type_traits>
#include <lua/lua.hpp>
 
template<typename>
struct AlwaysFalse : std::false_type {};

template<typename T>
auto LuaGetClass(lua_State* L, LPCSTR lname, T& value) -> bool
{
    auto* ptr = static_cast<T*>(luaL_testudata(L, 1, lname));
    luaL_argexpected(L, ptr != nullptr, 1, lname);

    if (ptr == nullptr)
        return false;

    value = *ptr;
    return true;
}

template<typename T>
auto LuaGet(lua_State* L, T& value) -> bool
{
    if (lua_gettop(L) < 1)
        return false;

    // builtins
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, DWORD> || std::is_same_v<T, uint32_t> || std::is_same_v<T, short>)
        value = static_cast<T>(luaL_checkinteger(L, 1));
    else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
        value = static_cast<T>(luaL_checknumber(L, 1));
    else if constexpr (std::is_same_v<T, bool>)
        value = static_cast<T>(lua_toboolean(L, 1));
    else if constexpr (std::is_same_v<T, LPCSTR>)
        value = static_cast<T>(luaL_checkstring(L, 1));
    else if constexpr (std::is_same_v<T, CString>)
        value = CString(static_cast<LPCSTR>(luaL_checkstring(L, 1)));
    else if constexpr (std::is_same_v<T, LPDIRECT3DTEXTURE9>)
        value = static_cast<LPDIRECT3DTEXTURE9>(lua_touserdata(L, 1));

    // modules
    else if constexpr (std::is_same_v<T, D3DXMATRIX>)
        LuaGetClass<T>(L, L_MATRIX, value);
    else if constexpr (std::is_same_v<T, VERTEX>)
        LuaGetClass<T>(L, L_VERTEX, value);
    else if constexpr (std::is_same_v<T, D3DXVECTOR3> || std::is_same_v<T, D3DXVECTOR4>)
        LuaGetClass<T>(L, L_VECTOR, value);
    else if constexpr (std::is_same_v<T, CMaterial*>)
        LuaGetClass<T>(L, L_MATERIAL, value);
    else if constexpr (std::is_same_v<T, CFaceGroup*>)
        LuaGetClass<T>(L, L_FACEGROUP, value);
    else if constexpr (std::is_same_v<T, CMesh*>)
        LuaGetClass<T>(L, L_MESH, value);
    else if constexpr (std::is_same_v<T, CScene*>)
        LuaGetClass<T>(L, L_SCENE, value);
    else if constexpr (std::is_same_v<T, CNode*>)
        LuaGetClass<T>(L, L_NODE, value);
    else if constexpr (std::is_same_v<T, CEffect*>)
        LuaGetClass<T>(L, L_EFFECT, value);
    else if constexpr (std::is_same_v<T, CRenderTarget*>)
        LuaGetClass<T>(L, L_RENDERTARGET, value);
    else if constexpr (std::is_same_v<T, CLight*>)
        LuaGetClass<T>(L, L_LIGHT, value);
    else if constexpr (std::is_same_v<T, CFont*>)
        LuaGetClass<T>(L, L_FONT, value);
    else if constexpr (std::is_same_v<T, CSound*>)
        LuaGetClass<T>(L, L_SOUND, value);
    else if constexpr (std::is_same_v<T, CMusic*>)
        LuaGetClass<T>(L, L_MUSIC, value);

    // unknown type, error out
    else
    {
        static_assert(AlwaysFalse<T>::value, "Type not implemented!");
        return false;
    }
 
    lua_remove(L, 1);
    return true;
}

template<typename T>
auto LuaGetInline(lua_State* L) -> T
{
    T value{};
    LuaGet<T>(L, value);
    return value;
}

inline auto LuaTestClass(lua_State* L, LPCSTR lname) -> bool
{
    if (lua_gettop(L) == 0)
        return false;

    return luaL_testudata(L, 1, lname) != nullptr;
}

inline auto LuaLength(lua_State* L) -> int
{
    return lua_gettop(L);
}
