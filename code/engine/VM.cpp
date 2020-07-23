#include "stdafx.h"
#include "VM.h"

#include "StdAfx.h"

#include "Engine.h"
#include "FileSystem.h"
#include "UserInterface.h"

#include "LuaBindings.h"

#include "ReferenceManager.h"

#include <lua/lua.hpp>
#include <lua/lstate.h>
#include <cstdio>

CVirtualMachine::CVirtualMachine(VOID)
{
	mPlayKind = PLAYKIND_STOPPED;
	mMainScript = NULL;
	mLuaVM = NULL;
	mScheduledTermination = FALSE;
	mRunTime = 0.0f;
}

VOID CVirtualMachine::Release(VOID)
{
	FILESYSTEM->FreeResource(mMainScript);
	mMainScript = NULL;
	DestroyVM();
}

/// States
VOID CVirtualMachine::Play(VOID)
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
	else if (mPlayKind == PLAYKIND_PAUSED)
    {
		mPlayKind = PLAYKIND_PLAYING;
		return;
    }

	if (mPlayKind != PLAYKIND_STOPPED)
		return;

	UI->ClearErrorWindow();

	FDATA f = FILESYSTEM->GetResource(RESOURCEKIND_SCRIPT);

	if (!f.data)
	{
		MessageBoxA(NULL, "No game script found!", "Resource error", MB_OK);
		ENGINE->Shutdown();
		return;
	}

	mMainScript = (UCHAR*)f.data;

	InitVM();
	mPlayKind = PLAYKIND_PLAYING;

	Init();
}

VOID CVirtualMachine::Pause(VOID)
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

VOID CVirtualMachine::Stop(VOID)
{
	if (mPlayKind == PLAYKIND_STOPPED)
		return;

	mScheduledTermination = TRUE;
	mPlayKind = PLAYKIND_STOPPED;
}

VOID CVirtualMachine::Restart(VOID)
{
	if (mPlayKind != PLAYKIND_STOPPED)
        Stop();

	// Request immediate restart upon termination.
    mScheduledTermination = 2;
}

/// Events
VOID CVirtualMachine::Init(VOID)
{
    if (!mLuaVM)
        return;

	mRunTime = 0.0f;

	lua_getglobal(mLuaVM, "_init");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	int r = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(r);
}

VOID CVirtualMachine::Destroy(VOID)
{
    if (!mLuaVM)
        return;

	lua_getglobal(mLuaVM, "_destroy");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	int r = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(r);
}

VOID CVirtualMachine::Update(FLOAT dt)
{
	if (mScheduledTermination)
	{
		UCHAR term = mScheduledTermination;
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

	int r = lua_pcall(mLuaVM, 1, 0, 0);
	CheckVMErrors(r);

	PassTime(dt);
}

VOID CVirtualMachine::Render(VOID)
{
    if (!mLuaVM || mPlayKind != PLAYKIND_PLAYING)
        return;

	lua_getglobal(mLuaVM, "_render");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	int r = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(r);
}

VOID CVirtualMachine::Render2D(VOID)
{
    if (!mLuaVM || mPlayKind != PLAYKIND_PLAYING)
        return;

    lua_getglobal(mLuaVM, "_render2d");

    if (!lua_isfunction(mLuaVM, -1))
        return;

    INT r = lua_pcall(mLuaVM, 0, 0, 0);
    CheckVMErrors(r);
}

VOID CVirtualMachine::Resize(RECT res)
{
    if (!mLuaVM || mPlayKind != PLAYKIND_PLAYING)
        return;

	lua_getglobal(mLuaVM, "_resizeScreen");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	lua_pushnumber(mLuaVM, res.right);
	lua_pushnumber(mLuaVM, res.bottom);

	int r = lua_pcall(mLuaVM, 2, 0, 0);
	CheckVMErrors(r);
}

VOID CVirtualMachine::CharInput(DWORD key)
{
	if (!mLuaVM || mPlayKind != PLAYKIND_PLAYING)
		return;

	lua_getglobal(mLuaVM, "_charInput");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	CHAR buf[2] = { (CHAR)key, 0 };
	lua_pushstring(mLuaVM, buf);

	int r = lua_pcall(mLuaVM, 1, 0, 0);
	CheckVMErrors(r);
}

static const luaL_Reg loadedlibs[] = {
	{"_G", luaopen_base},
	{LUA_LOADLIBNAME, luaopen_package},
	{LUA_COLIBNAME, luaopen_coroutine},
	{LUA_TABLIBNAME, luaopen_table},
	//{LUA_IOLIBNAME, luaopen_io},
	//{LUA_OSLIBNAME, luaopen_os},
	{LUA_STRLIBNAME, luaopen_string},
	{LUA_MATHLIBNAME, luaopen_math},
	//{LUA_UTF8LIBNAME, luaopen_utf8},
	{LUA_DBLIBNAME, luaopen_debug},
#if defined(LUA_COMPAT_BITLIB)
	{LUA_BITLIBNAME, luaopen_bit32},
#endif
	{NULL, NULL}
};

static VOID _lua_openlibs(lua_State *L) {
	const luaL_Reg *lib;

	for (lib = loadedlibs; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);
	}
	
	static char path[MAX_PATH] = { 0 };
	sprintf_s(path, MAX_PATH, "package.path = '%s/?/init.lua;libs/?/init.lua;%s/?.lua'", FILESYSTEM->GetCanonicalGamePath(), FILESYSTEM->GetCanonicalGamePath());
	luaL_dostring(L, path);
    sprintf_s(path, MAX_PATH, "package.cpath = '%s/?.dll;libs/?.dll;libs/?/init.dll;libs/?/?.dll'", FILESYSTEM->GetCanonicalGamePath());
    luaL_dostring(L, path);
}

static INT neon_luapanic(lua_State* L) {
    lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n",
        lua_tostring(L, -1));
    return 0;  /* return to Lua to abort */
}

VOID CVirtualMachine::InitVM(VOID)
{
	INT result;
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
	result = luaL_loadstring(mLuaVM, (const char*)mMainScript);
	CheckVMErrors(result, TRUE);

	result = lua_pcall(mLuaVM, 0, 0, 0);
	if (CheckVMErrors(result)) return;
}

VOID CVirtualMachine::DestroyVM(VOID)
{
	if (!mLuaVM)
		return;

	lua_close(mLuaVM);
	mLuaVM = NULL;
}

VOID CVirtualMachine::PrintVMError()
{
	const char* msg = lua_tostring(mLuaVM, -1);
#ifdef _DEBUG
	UI->PushErrorMessage(msg);
#else
    MessageBoxA(NULL, msg, "Lua error", MB_OK);
    ENGINE->Shutdown();
#endif	
}

BOOL CVirtualMachine::CheckVMErrors(INT result, BOOL canFail)
{
	gMemUsedLua = mLuaVM->l_G->totalbytes;

	if (result != LUA_OK)
	{
		PrintVMError();

#ifdef _DEBUG
		if (!canFail)
		{
			VM->Stop();
			VM->Release();
		}
#endif
		return TRUE;
	}

	return FALSE;
}

inline VOID CVirtualMachine::PostError(LPCSTR err)
{
#ifdef _DEBUG
    UI->PushErrorMessage(err);
	
	if (mLuaVM)
		lua_error(mLuaVM);
#else
    MessageBoxA(NULL, err, "Engine error", MB_OK);
    ENGINE->Shutdown();
#endif
}

inline VOID CVirtualMachine::PostError(CString err)
{
	PostError(err.Str());
}
