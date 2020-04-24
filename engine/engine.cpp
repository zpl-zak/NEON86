// engine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "engine.h"

#include "Renderer.h"
#include "Input.h"
#include "FileSystem.h"
#include "LuaMachine.h"
#include "UserInterface.h"
#include <ctime>

CEngine::CEngine()
: mIsRunning(FALSE)
{
	srand((size_t)time(0));
	sInstance = this; 
	mIsInitialised = FALSE;
	mRenderer = NULL;
	mInput = NULL;
	mFileSystem = NULL;
	mLuaMachine = NULL;
	mUserInterface = NULL;
	
	SetFPS(60.0f);
	mUnprocessedTime = 0.0f;
	mLastTime = 0.0f;

	return; 
}

BOOL CEngine::Release()
{
	SAFE_RELEASE(mLuaMachine);
	SAFE_RELEASE(mFileSystem);
	SAFE_RELEASE(mUserInterface);
	SAFE_RELEASE(mRenderer);
	SAFE_RELEASE(mInput);

	return TRUE;
}

VOID CEngine::Shutdown()
{
	mIsRunning = FALSE;
}

CEngine* CEngine::the()
{
	if (!sInstance)
		sInstance = new CEngine();

	return sInstance;
}

BOOL CEngine::Init(HWND window, RECT resolution)
{
	if (mIsInitialised)
		return TRUE;

	mRenderer = new CRenderer();
	mUserInterface = new CUserInterface();
	mInput = new CInput();
	mFileSystem = new CFileSystem();
	mLuaMachine = new CLuaMachine();

	if (mRenderer->CreateDevice(window) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	mRenderer->Resize(resolution);

	mIsInitialised = TRUE;
	mIsRunning = TRUE;

	return TRUE;
}

VOID CEngine::Think()
{
	BOOL render = FALSE;
	FLOAT startTime = GetTime();
	FLOAT deltaTime = startTime - mLastTime;
	mLastTime = startTime;

	mUnprocessedTime += deltaTime;

	while (mUnprocessedTime > mUpdateDuration)
	{
		Update(mUpdateDuration);
		render = TRUE;
		mUnprocessedTime -= mUpdateDuration;
	}

	if (render)
		Render();
	else
		Sleep(1); // Let CPU sleep a bit
}

VOID CEngine::Update(FLOAT deltaTime)
{
	mLuaMachine->Update(deltaTime);
	mInput->Update();
}

VOID CEngine::Render()
{
	mRenderer->GetDevice()->BeginScene();
	mLuaMachine->Render();
	mLuaMachine->Render2D();
	mRenderer->GetDevice()->EndScene();
	mRenderer->GetDevice()->Present(NULL, NULL, NULL, NULL);
}

VOID CEngine::Resize(RECT resolution)
{
	if (!mIsInitialised)
		return;
	mLuaMachine->Resize(resolution);
	mRenderer->Resize(resolution);
}

CEngine *CEngine::sInstance = NULL;
