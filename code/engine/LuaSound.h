#include <lua/lua.hpp>

INT sound_new(lua_State* L)
{
    LPSTR wavPath = (LPSTR)luaL_checkstring(L, 1);
    *(CSound**)lua_newuserdata(L, sizeof(CSound*)) = new CSound(wavPath);
    luaL_setmetatable(L, L_SOUND);
    return 1;
}

INT sound_play(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    snd->Play();
    return 0;
}

INT sound_pause(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    snd->Pause();
    return 0;
}

INT sound_stop(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    snd->Stop();
    return 0;
}

INT sound_getdata(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    ULONG dataLen = 0;
    UCHAR* data = snd->GetData(&dataLen);

    lua_newtable(L);

    for (ULONG i=0; i<dataLen; i++)
    {
        lua_pushinteger(L, i + 1);
        lua_pushinteger(L, data[i]);
        lua_settable(L, -3);
    }
    return 1;
}

INT sound_setvolume(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    LONG vol = (LONG)luaL_checkinteger(L, 2);
    snd->SetVolume(vol);
    return 0;
}

INT sound_setpan(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    LONG pan = (LONG)luaL_checkinteger(L, 2);
    snd->SetPan(pan);
    return 0;
}

INT sound_setcursor(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    DWORD pos = (DWORD)luaL_checkinteger(L, 2);
    snd->SetCurrentPosition(pos);
    return 0;
}

INT sound_getvolume(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    lua_pushinteger(L, snd->GetVolume());
    return 1;
}

INT sound_getpan(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    lua_pushinteger(L, snd->GetPan());
    return 1;
}

INT sound_getcursor(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    lua_pushinteger(L, snd->GetCurrentPosition());
    return 1;
}

INT sound_playing(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    lua_pushboolean(L, snd->IsPlaying());
    return 1;
}

INT sound_setloop(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    BOOL loop = (BOOL)lua_toboolean(L, 2);
    snd->SetLoop(loop);
    return 0;
}

INT sound_looping(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    lua_pushboolean(L, snd->IsLooping());
    return 1;
}

static INT sound_delete(lua_State* L)
{
    CSound* snd = *(CSound**)luaL_checkudata(L, 1, L_SOUND);
    snd->Release();

    return 0;
}

static VOID LuaSound_register(lua_State* L)
{
    lua_register(L, L_SOUND, sound_new);
    luaL_newmetatable(L, L_SOUND);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    REGC("play", sound_play);
    REGC("pause", sound_pause);
    REGC("stop", sound_stop);
    REGC("setVolume", sound_setvolume);
    REGC("setPan", sound_setpan);
    REGC("setCursor", sound_setcursor);
    REGC("loop", sound_setloop);
    REGC("isLooping", sound_looping);
    REGC("isPlaying", sound_playing);
    REGC("getVolume", sound_getvolume);
    REGC("getPan", sound_getpan);
    REGC("getCursor", sound_getcursor);
    REGC("getData", sound_getdata);
    REGC("__gc", sound_delete);

    lua_pop(L, 1);
}
