#include <lua/lua.hpp>

auto sound_new(lua_State* L) -> INT
{
    const auto wavPath = (LPSTR)luaL_checkstring(L, 1);
    *static_cast<CSound**>(lua_newuserdata(L, sizeof(CSound*))) = new CSound(wavPath);
    luaL_setmetatable(L, L_SOUND);
    return 1;
}

auto sound_play(lua_State* L) -> INT
{
    auto snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    snd->Play();
    return 0;
}

auto sound_pause(lua_State* L) -> INT
{
    auto snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    snd->Pause();
    return 0;
}

auto sound_stop(lua_State* L) -> INT
{
    auto snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    snd->Stop();
    return 0;
}

auto sound_getdata(lua_State* L) -> INT
{
    auto snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    ULONG dataLen = 0;
    auto data = snd->GetData(&dataLen);

    lua_newtable(L);

    for (ULONG i = 0; i < dataLen; i++)
    {
        lua_pushinteger(L, i + 1);
        lua_pushinteger(L, data[i]);
        lua_settable(L, -3);
    }
    return 1;
}

auto sound_setvolume(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    const LONG vol = static_cast<LONG>(luaL_checkinteger(L, 2));
    snd->SetVolume(vol);
    return 0;
}

auto sound_setpan(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    const LONG pan = static_cast<LONG>(luaL_checkinteger(L, 2));
    snd->SetPan(pan);
    return 0;
}

auto sound_setfrequency(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    const auto val = static_cast<DWORD>(luaL_checkinteger(L, 2));
    snd->SetFrequency(val);
    return 0;
}

auto sound_setcursor(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    const auto pos = static_cast<DWORD>(luaL_checkinteger(L, 2));
    snd->SetCurrentPosition(pos);
    return 0;
}

auto sound_getvolume(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    lua_pushinteger(L, snd->GetVolume());
    return 1;
}

auto sound_getpan(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    lua_pushinteger(L, snd->GetPan());
    return 1;
}

auto sound_getfrequency(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    lua_pushinteger(L, snd->GetFrequency());
    return 1;
}

auto sound_getcursor(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    lua_pushinteger(L, snd->GetCurrentPosition());
    return 1;
}

auto sound_playing(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    lua_pushboolean(L, snd->IsPlaying());
    return 1;
}

auto sound_setloop(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    const BOOL loop = static_cast<BOOL>(lua_toboolean(L, 2));
    snd->SetLoop(loop);
    return 0;
}

auto sound_looping(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    lua_pushboolean(L, snd->IsLooping());
    return 1;
}

auto sound_setpos(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    const auto pos = static_cast<DWORD>(luaL_checkinteger(L, 2));
    snd->SetCurrentPosition(pos);
    return 0;
}

auto sound_getpos(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    lua_pushinteger(L, snd->GetCurrentPosition());
    return 1;
}

auto sound_getsize(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    lua_pushinteger(L, snd->GetTotalSize());
    return 1;
}

static auto sound_delete(lua_State* L) -> INT
{
    CSound* snd = *static_cast<CSound**>(luaL_checkudata(L, 1, L_SOUND));
    snd->Release();

    return 0;
}

static VOID LuaSound_register(lua_State* L)
{
    lua_register(L, L_SOUND, sound_new);
    luaL_newmetatable(L, L_SOUND);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("play", sound_play);
    REGC("pause", sound_pause);
    REGC("stop", sound_stop);
    REGC("setVolume", sound_setvolume);
    REGC("setPan", sound_setpan);
    REGC("setFrequency", sound_setfrequency);
    REGC("setCursor", sound_setcursor);
    REGC("loop", sound_setloop);
    REGC("isLooping", sound_looping);
    REGC("isPlaying", sound_playing);
    REGC("getVolume", sound_getvolume);
    REGC("getPan", sound_getpan);
    REGC("getFrequency", sound_getfrequency);
    REGC("getCursor", sound_getcursor);
    REGC("getData", sound_getdata);
    REGC("setPosition", sound_setpos);
    REGC("getPosition", sound_getpos);
    REGC("getTotalSize", sound_getsize);
    REGC("__gc", sound_delete);

    lua_pop(L, 1);
}
