#include "stdafx.h"
#include "LuaMachine.h"

#include "StdAfx.h"
#include "NeonEngine.h"
#include "LuaBindings.h"

#include "ReferenceManager.h"

#include <lua/lua.hpp>

CLuaMachine::CLuaMachine(void)
{
	mPlayKind = PLAYKIND_STOPPED;
	mMainScript = NULL;
	mLuaVM = NULL;
	mScheduledTermination = FALSE;
}

VOID CLuaMachine::Release(void)
{
	CReferenceManager::Release();
	FILESYSTEM->FreeResource(mMainScript);
	mMainScript = NULL;
	DestroyVM();
}

/// States
VOID CLuaMachine::Play(void)
{
	if (mScheduledTermination)
	{
		return;
	}

	if (mPlayKind != PLAYKIND_STOPPED)
		return;

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

VOID CLuaMachine::Pause(void)
{
	if (mPlayKind == PLAYKIND_PLAYING)
		mPlayKind = PLAYKIND_PAUSED;
	else if (mPlayKind == PLAYKIND_PAUSED)
		mPlayKind = PLAYKIND_PLAYING;
}

VOID CLuaMachine::Stop(void)
{
	if (mPlayKind == PLAYKIND_STOPPED)
		return;

	mScheduledTermination = TRUE;
	mPlayKind = PLAYKIND_STOPPED;
}

VOID CLuaMachine::Restart(void)
{
	if (mPlayKind != PLAYKIND_STOPPED)
        Stop();

	// Request immediate restart upon termination.
    mScheduledTermination = 2;
}

/// Events
VOID CLuaMachine::Init(void)
{
    if (!mLuaVM)
        return;

	ENGINE->ResetApplicationTime();

	lua_getglobal(mLuaVM, "_init");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	int r = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(r);
}

VOID CLuaMachine::Destroy(void)
{
    if (!mLuaVM)
        return;

	lua_getglobal(mLuaVM, "_destroy");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	int r = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(r);
}

VOID CLuaMachine::Update(FLOAT dt)
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
}

VOID CLuaMachine::Render(void)
{
	if (!mLuaVM)
		return;

	lua_getglobal(mLuaVM, "_render");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	int r = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(r);
}

VOID CLuaMachine::Render2D(void)
{
    if (!mLuaVM)
        return;

    lua_getglobal(mLuaVM, "_render2d");

    if (!lua_isfunction(mLuaVM, -1))
        return;

    INT r = lua_pcall(mLuaVM, 0, 0, 0);
    CheckVMErrors(r);
}

VOID CLuaMachine::Resize(RECT res)
{
    if (!mLuaVM)
        return;

	lua_getglobal(mLuaVM, "_resizeScreen");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	lua_pushnumber(mLuaVM, res.right);
	lua_pushnumber(mLuaVM, res.bottom);

	int r = lua_pcall(mLuaVM, 2, 0, 0);
	CheckVMErrors(r);
}

static const luaL_Reg loadedlibs[] = {
	{"_G", luaopen_base},
	//{LUA_LOADLIBNAME, luaopen_package},
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

static void _lua_openlibs(lua_State *L) {
	const luaL_Reg *lib;

	for (lib = loadedlibs; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);
	}
}

VOID CLuaMachine::InitVM(void)
{
	INT result;
	mLuaVM = luaL_newstate();

	_lua_openlibs(mLuaVM);
	
	/// Bindings
	CLuaBindings::BindBase(mLuaVM);
	CLuaBindings::BindMath(mLuaVM);
	CLuaBindings::BindRenderer(mLuaVM);
	CLuaBindings::BindInput(mLuaVM);

	// Load script
	result = luaL_loadstring(mLuaVM, (const char*)mMainScript);
	CheckVMErrors(result);

	result = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(result);
}

VOID CLuaMachine::DestroyVM(void)
{
	if (!mLuaVM)
		return;

	lua_close(mLuaVM);
	mLuaVM = NULL;
}

void CLuaMachine::PrintVMError()
{
	const char* msg = lua_tostring(mLuaVM, -1);
	MessageBoxA(NULL, msg, "Lua error", MB_OK);
	ENGINE->Shutdown();
}

void CLuaMachine::CheckVMErrors(INT result)
{
	if (result != LUA_OK)
	{
		PrintVMError();
	}
}