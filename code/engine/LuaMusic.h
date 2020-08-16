#include <lua/lua.hpp>

INT music_new(lua_State* L)
{
    LPSTR path = (LPSTR)luaL_checkstring(L, 1);
    *(CMusic**)lua_newuserdata(L, sizeof(CMusic*)) = new CMusic(path);
    luaL_setmetatable(L, L_MUSIC);
    return 1;
}

INT music_play(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    snd->Play();
    return 0;
}

INT music_pause(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    snd->Pause();
    return 0;
}

INT music_stop(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    snd->Stop();
    return 0;
}

INT music_setvolume(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    LONG vol = (LONG)luaL_checkinteger(L, 2);
    snd->SetVolume(vol);
    return 0;
}

INT music_setpan(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    LONG pan = (LONG)luaL_checkinteger(L, 2);
    snd->SetPan(pan);
    return 0;
}

INT music_getvolume(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    lua_pushinteger(L, snd->GetVolume());
    return 1;
}

INT music_getpan(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    lua_pushinteger(L, snd->GetPan());
    return 1;
}

INT music_playing(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    lua_pushboolean(L, snd->IsPlaying());
    return 1;
}

INT music_setpos(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    DWORD pos = (DWORD)luaL_checkinteger(L, 2);
    snd->SetCurrentPosition(pos);
    return 0;
}

INT music_getpos(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    lua_pushinteger(L, snd->GetCurrentPosition());
    return 1;
}

INT music_getsize(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    lua_pushinteger(L, snd->GetTotalSize());
    return 1;
}

static INT music_delete(lua_State* L)
{
    CMusic* snd = *(CMusic**)luaL_checkudata(L, 1, L_MUSIC);
    snd->Release();

    return 0;
}

static VOID LuaMusic_register(lua_State* L)
{
    lua_register(L, L_MUSIC, music_new);
    luaL_newmetatable(L, L_MUSIC);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    REGC("play", music_play);
    REGC("pause", music_pause);
    REGC("stop", music_stop);
    REGC("setVolume", music_setvolume);
    REGC("setPan", music_setpan);
    REGC("isPlaying", music_playing);
    REGC("getVolume", music_getvolume);
    REGC("getPan", music_getpan);
    REGC("setPosition", music_setpos);
    REGC("getPosition", music_getpos);
    REGC("getTotalSize", music_getsize);
    REGC("__gc", music_delete);

    lua_pop(L, 1);
}
