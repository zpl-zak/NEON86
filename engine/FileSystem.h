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
	RESOURCEKIND_USER,
	RESOURCEKIND_IMAGE,
};

struct FDATA
{
	VOID* data;
	UINT size;
};

#define RESOURCE_META "meta.yaml"
#define RESOURCE_SCRIPT "init.lua"

#ifndef _FILE_DEFINED
#define _FILE_DEFINED
typedef struct _iobuf
{
    void* _Placeholder;
} FILE;
#endif

#define FILESYSTEM CEngine::the()->GetFileSystem()

class ENGINE_API CFileSystem
{
public:
	CFileSystem(void);
	BOOL LoadGame(LPSTR gamePath, UCHAR loadKind = LOADKIND_FOLDER);
	FDATA GetResource(UCHAR kind, LPCSTR resName=NULL);
	FILE* OpenResource(UCHAR kind, LPCSTR resName = NULL);
	VOID CloseResource(FILE* handle);
	LPSTR ResourcePath(UCHAR kind, LPCSTR resName = NULL);
	BOOL Exists(UCHAR kind, LPCSTR resName);
	VOID FreeResource(VOID* data);
	VOID Release(void);

private:
	UCHAR mLoadKind;
	LPSTR mGamePath;
	BOOL mLoadDone;

	BOOL LoadGameInternal();
	VOID FixName(UCHAR kind, LPCSTR* resName);
};
