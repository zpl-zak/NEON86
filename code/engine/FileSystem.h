#pragma once

#include "system.h"

enum RESOURCEKIND
{
	RESOURCEKIND_META,
	RESOURCEKIND_TEXT,
	RESOURCEKIND_SCRIPT,
	RESOURCEKIND_USER,
	RESOURCEKIND_UDATA,
	RESOURCEKIND_IMAGE,
};

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
	FDATA GetResource(UCHAR kind, LPCSTR resName=NULL);
	VOID SaveResource(UCHAR kind, LPCSTR data, UINT64 size);
	FILE* OpenResource(UCHAR kind, LPCSTR resName = NULL);
	VOID CloseResource(FILE* handle);
	LPCSTR ResourcePath(UCHAR kind, LPCSTR resName = NULL);
	LPCSTR GetGamePath() { return mGamePath; }
	LPCSTR GetCanonicalGamePath();
	BOOL Exists(UCHAR kind, LPCSTR resName);
	VOID FreeResource(LPVOID data);
	VOID Release(VOID);

private:
	LPSTR mGamePath;
	BOOL mLoadDone;

	BOOL LoadGameInternal();
	VOID FixName(UCHAR kind, LPCSTR* resName);
};
