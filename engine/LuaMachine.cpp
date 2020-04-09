#include "stdafx.h"
#include "LuaMachine.h"

#include "StdAfx.h"
#include "NeonEngine.h"
#include "LuaBindings.h"

#include <lua/lua.hpp>

CLuaMachine::CLuaMachine(void)
{
	mPlayKind = PLAYKIND_STOPPED;
	mMainScript = NULL;
	mLuaVM = NULL;
}

VOID CLuaMachine::Release(void)
{
	FILESYSTEM->FreeResource(mMainScript);
	mMainScript = NULL;
	DestroyVM();
}

/// States
VOID CLuaMachine::Play(void)
{
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

	Destroy();
	Release();
	mPlayKind = PLAYKIND_STOPPED;
}

VOID CLuaMachine::Restart(void)
{
	if (mPlayKind != PLAYKIND_STOPPED)
		Stop();

	Play();
}

/// Events
VOID CLuaMachine::Init(void)
{
	lua_getglobal(mLuaVM, "_init");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	int r = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(r);
}

VOID CLuaMachine::Destroy(void)
{
	lua_getglobal(mLuaVM, "_destroy");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	int r = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(r);
}

VOID CLuaMachine::Update(FLOAT dt)
{
	lua_getglobal(mLuaVM, "_update");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	lua_pushnumber(mLuaVM, dt);

	int r = lua_pcall(mLuaVM, 1, 0, 0);
	CheckVMErrors(r);
}

VOID CLuaMachine::Render(void)
{
	lua_getglobal(mLuaVM, "_render");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	int r = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(r);
}

VOID CLuaMachine::Resize(RECT res)
{
	lua_getglobal(mLuaVM, "_resizeScreen");

	if (!lua_isfunction(mLuaVM, -1))
		return;

	lua_pushnumber(mLuaVM, res.right);
	lua_pushnumber(mLuaVM, res.bottom);

	int r = lua_pcall(mLuaVM, 2, 0, 0);
	CheckVMErrors(r);
}

VOID CLuaMachine::InitVM(void)
{
	INT result;
	mLuaVM = luaL_newstate();

	luaL_openlibs(mLuaVM);
	
	result = luaL_loadstring(mLuaVM, (const char*)mMainScript);
	CheckVMErrors(result);

	result = lua_pcall(mLuaVM, 0, 0, 0);
	CheckVMErrors(result);

	/// Bindings
	CLuaBindings::BindBase(mLuaVM);
	CLuaBindings::BindMath(mLuaVM);
	CLuaBindings::BindRenderer(mLuaVM);
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