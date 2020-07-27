#pragma once

#include "system.h"

struct FDATA
{
	LPVOID data;
	UINT size;
};

#define RESOURCE_UDATA "save.neon"
#define RESOURCE_SCRIPT "init.lua"

#ifndef _FILE_DEFINED
#define _FILE_DEFINED
typedef struct _iobuf
{
    LPVOID _Placeholder;
} FILE;
#endif

#define FILESYSTEM CEngine::the()->GetFileSystem()

class ENGINE_API CFileSystem
{
public:
	CFileSystem(VOID);
	BOOL LoadGame(LPSTR gamePath);
	FDATA GetResource(LPCSTR resName=NULL);
	VOID SaveResource(LPCSTR data, UINT64 size);
	FILE* OpenResource(LPCSTR resName = NULL);
	VOID CloseResource(FILE* handle);
	LPCSTR ResourcePath(LPCSTR resName = NULL);
	LPCSTR GetGamePath() { return mGamePath; }
	LPCSTR GetCanonicalGamePath();
	BOOL Exists(LPCSTR resName);
	VOID FreeResource(LPVOID data);
	VOID Release(VOID);

private:
	LPSTR mGamePath;
	BOOL mLoadDone;

	BOOL LoadGameInternal();
	VOID FixName(LPCSTR* resName);
};
