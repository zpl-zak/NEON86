#pragma once

#include <lua/lua.hpp>
#include <type_traits>
 
template<typename>
struct always_false : std::false_type {};

template<typename T>
auto LuaGetClass(lua_State* L, LPCSTR lname, T& value) -> bool
{
    auto* ptr = static_cast<T*>(luaL_testudata(L, 1, lname));
    luaL_argexpected(L, ptr != nullptr, 1, lname);

    if (!ptr)
        return false;

    value = *ptr;
    return true;
}

template<typename T>
inline auto LuaGet(lua_State* L, T& value) -> bool
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
        value = CString(static_cast<T>(luaL_checkstring(L, 1)));
    else if constexpr (std::is_same_v<T, LPDIRECT3DTEXTURE9>)
        value = static_cast<LPDIRECT3DTEXTURE9>(lua_touserdata(L, 1));

    // modules
    else if constexpr (std::is_same_v<T, D3DXMATRIX>)
        LuaGetClass<D3DXMATRIX>(L, L_MATRIX, value);
    else if constexpr (std::is_same_v<T, VERTEX>)
        LuaGetClass<VERTEX>(L, L_VERTEX, value);
    else if constexpr (std::is_same_v<T, D3DXVECTOR3>)
        LuaGetClass<D3DXVECTOR3>(L, L_VECTOR, value);
    else if constexpr (std::is_same_v<T, D3DXVECTOR4>)
        LuaGetClass<D3DXVECTOR4>(L, L_VECTOR, value);
    else if constexpr (std::is_same_v<T, CMaterial*>)
        LuaGetClass<CMaterial*>(L, L_MATERIAL, value);
    else if constexpr (std::is_same_v<T, CFaceGroup*>)
        LuaGetClass<CFaceGroup*>(L, L_FACEGROUP, value);
    else if constexpr (std::is_same_v<T, CMesh*>)
        LuaGetClass<CMesh*>(L, L_MESH, value);
    else if constexpr (std::is_same_v<T, CScene*>)
        LuaGetClass<CScene*>(L, L_SCENE, value);
    else if constexpr (std::is_same_v<T, CNode*>)
        LuaGetClass<CNode*>(L, L_NODE, value);
    else if constexpr (std::is_same_v<T, CEffect*>)
        LuaGetClass<CEffect*>(L, L_EFFECT, value);
    else if constexpr (std::is_same_v<T, CRenderTarget*>)
        LuaGetClass<CRenderTarget*>(L, L_RENDERTARGET, value);
    else if constexpr (std::is_same_v<T, CLight*>)
        LuaGetClass<CLight*>(L, L_LIGHT, value);
    else if constexpr (std::is_same_v<T, CFont*>)
        LuaGetClass<CFont*>(L, L_FONT, value);
    else if constexpr (std::is_same_v<T, CSound*>)
        LuaGetClass<CSound*>(L, L_SOUND, value);
    else if constexpr (std::is_same_v<T, CMusic*>)
        LuaGetClass<CMusic*>(L, L_MUSIC, value);

    // unknown type, error out
    else
    {
        static_assert(always_false<T>::value, "Type not implemented!");
        return false;
    }
 
    lua_remove(L, 1);
    return true;
}

template<typename T>
inline auto LuaGetInline(lua_State* L) -> T
{
    T value{};
    LuaGet<T>(L, value);
    return value;
}

auto LuaTestClass(lua_State* L, LPCSTR lname) -> bool
{
    return luaL_testudata(L, 1, lname) != nullptr;
}

auto LuaLength(lua_State* L) -> int
{
    return lua_gettop(L);
}

void LuaPop(lua_State* L, int idx)
{
    lua_pop(L, idx);
}
