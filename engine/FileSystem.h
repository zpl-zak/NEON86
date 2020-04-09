#pragma once

#include "system.h"

enum LOADKIND
{
	LOADKIND_FOLDER,
	LOADKIND_PAK,
};

enum RESOURCEKIND
{
	RESOURCEKIND_META,
	RESOURCEKIND_TEXT,
	RESOURCEKIND_SCRIPT,
	RESOURCEKIND_IMAGE,
};

#define RESOURCE_META "meta.yaml"
#define RESOURCE_SCRIPT "init.lua"

#define FILESYSTEM CEngine::the()->GetFileSystem()

class ENGINE_API CFileSystem
{
public:
	CFileSystem(void);
	BOOL LoadGame(LPSTR gamePath, UCHAR loadKind = LOADKIND_FOLDER);
	VOID* GetResource(UCHAR kind, LPSTR resName=NULL);
	VOID FreeResource(VOID* data);
	VOID Release(void);

private:
	UCHAR mLoadKind;
	LPSTR mGamePath;
	BOOL mLoadDone;

	BOOL LoadGameInternal();
};
