#include "stdafx.h"
#include "VM.h"

#include "StdAfx.h"

#include "Engine.h"
#include "FileSystem.h"
#include "UserInterface.h"
#include "Renderer.h"

#include "LuaBindings.h"

#include "ReferenceManager.h"

#include <lua/lua.hpp>
#include <lua/lstate.h>
#include <cstdio>

CVirtualMachine::CVirtualMachine(void)
{
    mPlayKind = PLAYKIND_STOPPED;
    mMainScript = nullptr;
    mLuaVM = nullptr;
    mScheduledTermination = FALSE;
    mRunTime = 0.0f;
}

void CVirtualMachine::Release(void)
{
    FILESYSTEM->FreeResource(mMainScript);
    mMainScript = nullptr;
    DestroyVM();
}

/// States
void CVirtualMachine::Play(void)
{
    if (mScheduledTermination)
    {
        return;
    }

    if (mPlayKind == PLAYKIND_PLAYING)
    {
        mPlayKind = PLAYKIND_PAUSED;
        return;
    }
    if (mPlayKind == PLAYKIND_PAUSED)
    {
        mPlayKind = PLAYKIND_PLAYING;
        return;
    }

    if (mPlayKind != PLAYKIND_STOPPED)
        return;

    UI->ClearErrorWindow();

    const auto f = FILESYSTEM->GetResource(RESOURCE_SCRIPT);

    if (!f.data)
    {
        MessageBoxA(nullptr, "No game script found!", "Resource error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    mMainScript = static_cast<UCHAR*>(f.data);

    mPlayKind = PLAYKIND_PLAYING;
    InitVM();

    Init();
}

void CVirtualMachine::Pause(void)
{
    if (mPlayKind == PLAYKIND_PLAYING)
    {
        mPlayKind = PLAYKIND_PAUSED;
    }
    else if (mPlayKind == PLAYKIND_PAUSED)
    {
        mPlayKind = PLAYKIND_PLAYING;
    }
}

void CVirtualMachine::Stop(void)
{
    if (mPlayKind == PLAYKIND_STOPPED)
        return;

    mScheduledTermination = TRUE;
    mPlayKind = PLAYKIND_STOPPED;
}

void CVirtualMachine::Restart(void)
{
    if (mPlayKind != PLAYKIND_STOPPED)
        Stop();

    // Request immediate restart upon termination.
    mScheduledTermination = 2;
}

/// Events
void CVirtualMachine::Init(void)
{
    if (!mLuaVM)
        return;

    if (mPlayKind == PLAYKIND_STOPPED)
        return;

    mRunTime = 0.0f;

    lua_getglobal(mLuaVM, "_init");

    if (!lua_isfunction(mLuaVM, -1))
        return;

    const auto r = lua_pcall(mLuaVM, 0, 0, 0);
    CheckVMErrors(r);
}

void CVirtualMachine::Destroy(void)
{
    if (!mLuaVM)
        return;

    lua_getglobal(mLuaVM, "_destroy");

    if (!lua_isfunction(mLuaVM, -1))
        return;

    const auto r = lua_pcall(mLuaVM, 0, 0, 0);
    CheckVMErrors(r);
}

void CVirtualMachine::Update(float dt)
{
    if (mScheduledTermination)
    {
        const auto term = mScheduledTermination;
        Destroy();
        Release();

        mScheduledTermination = FALSE;

        if (term == 2) /* Restart was requested */
            Play();

        return;
    }

    if (!mLuaVM || mPlayKind != PLAYKIND_PLAYING)
        return;

    lua_getglobal(mLuaVM, "_update");

    if (!lua_isfunction(mLuaVM, -1))
        return;

    lua_pushnumber(mLuaVM, dt);

    const auto r = lua_pcall(mLuaVM, 1, 0, 0);
    CheckVMErrors(r);

    PassTime(dt);
}

void CVirtualMachine::Render(void)
{
    if (!mLuaVM || mPlayKind != PLAYKIND_PLAYING)
        return;

    lua_getglobal(mLuaVM, "_render");

    if (!lua_isfunction(mLuaVM, -1))
        return;

    const auto r = lua_pcall(mLuaVM, 0, 0, 0);
    CheckVMErrors(r);
}

void CVirtualMachine::Render2D(void)
{
    if (!mLuaVM || mPlayKind != PLAYKIND_PLAYING)
        return;

    lua_getglobal(mLuaVM, "_render2d");

    if (!lua_isfunction(mLuaVM, -1))
        return;

    const auto r = lua_pcall(mLuaVM, 0, 0, 0);
    CheckVMErrors(r);
}

void CVirtualMachine::Resize(RECT res)
{
    if (!mLuaVM || mPlayKind != PLAYKIND_PLAYING)
        return;

    lua_getglobal(mLuaVM, "_resizeScreen");

    if (!lua_isfunction(mLuaVM, -1))
        return;

    lua_pushnumber(mLuaVM, res.right);
    lua_pushnumber(mLuaVM, res.bottom);

    const auto r = lua_pcall(mLuaVM, 2, 0, 0);
    CheckVMErrors(r);
}

void CVirtualMachine::CharInput(DWORD key)
{
    if (!mLuaVM || mPlayKind != PLAYKIND_PLAYING)
        return;

    lua_getglobal(mLuaVM, "_charInput");

    if (!lua_isfunction(mLuaVM, -1))
        return;

    CHAR buf[2] = {static_cast<CHAR>(key), 0};
    lua_pushstring(mLuaVM, buf);

    const auto r = lua_pcall(mLuaVM, 1, 0, 0);
    CheckVMErrors(r);
}

static const luaL_Reg loadedlibs[] = {
    {"_G", luaopen_base},
    {LUA_LOADLIBNAME, luaopen_package},
    {LUA_COLIBNAME, luaopen_coroutine},
    {LUA_TABLIBNAME, luaopen_table},
    {LUA_IOLIBNAME, luaopen_io},
    {LUA_OSLIBNAME, luaopen_os},
    {LUA_STRLIBNAME, luaopen_string},
    {LUA_MATHLIBNAME, luaopen_math},
    {LUA_UTF8LIBNAME, luaopen_utf8},
    {LUA_DBLIBNAME, luaopen_debug},
    #if defined(LUA_COMPAT_BITLIB)
	{LUA_BITLIBNAME, luaopen_bit32},
    #endif
    {nullptr, nullptr}
};

static void _lua_openlibs(lua_State* L)
{
    for (const auto* lib = loadedlibs; lib->func; lib++)
    {
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);
    }

    static char path[MAX_PATH] = {0};
    sprintf_s(path, MAX_PATH, "package.path = '%s/?/init.lua;libs/?/init.lua;%s/?.lua;libs/?/?.lua'",
              FILESYSTEM->GetCanonicalGamePath(), FILESYSTEM->GetCanonicalGamePath());
    luaL_dostring(L, path);
    sprintf_s(path, MAX_PATH, "package.cpath = '%s/?.dll;libs/?.dll;libs/?/init.dll;libs/?/?.dll'",
              FILESYSTEM->GetCanonicalGamePath());
    luaL_dostring(L, path);
}

static auto neon_luapanic(lua_State* L) -> int
{
    lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n",
                         lua_tostring(L, -1));
    return 0; /* return to Lua to abort */
}

void CVirtualMachine::InitVM(void)
{
    mLuaVM = luaL_newstate();
    if (!mLuaVM) lua_atpanic(mLuaVM, &neon_luapanic);

    _lua_openlibs(mLuaVM);

    /// Bindings
    CLuaBindings::BindBase(mLuaVM);
    CLuaBindings::BindMath(mLuaVM);
    CLuaBindings::BindRenderer(mLuaVM);
    CLuaBindings::BindInput(mLuaVM);
    CLuaBindings::BindAudio(mLuaVM);

    // Load script
    auto result = luaL_loadstring(mLuaVM, (const char*)mMainScript);
    CheckVMErrors(result, TRUE);

    result = lua_pcall(mLuaVM, 0, 0, 0);
    if (CheckVMErrors(result)) return;
}

void CVirtualMachine::DestroyVM(void)
{
    if (!mLuaVM)
        return;

    lua_close(mLuaVM);
    mLuaVM = nullptr;
}

void CVirtualMachine::PrintVMError() const
{
    const auto msg = lua_tostring(mLuaVM, -1);
    #ifdef _DEBUG
    RENDERER->SetRenderTarget(nullptr);
    UI->PushErrorMessage(msg);
    #else
    MessageBoxA(NULL, msg, "Lua error", MB_OK);
    ENGINE->Shutdown();
    #endif
}

auto CVirtualMachine::CheckVMErrors(int result, bool canFail) -> bool
{
    gMemUsedLua = mLuaVM->l_G->totalbytes;

    if (result != LUA_OK)
    {
        PrintVMError();

        #ifdef _DEBUG
        if (!canFail)
            Pause();
        #endif
        return TRUE;
    }

    return FALSE;
}

inline void CVirtualMachine::PostError(LPCSTR err)
{
    #ifdef _DEBUG
    UI->PushErrorMessage(err);
    Pause();
    #else
    MessageBoxA(NULL, err, "Engine error", MB_OK);
    ENGINE->Shutdown();
    #endif
}

inline void CVirtualMachine::PostError(CString err)
{
    PostError(err.Str());
}
