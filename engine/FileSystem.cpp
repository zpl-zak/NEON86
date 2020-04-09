#include "stdafx.h"
#include "FileSystem.h"

#include <cstdio>
#include <cstdlib>

CFileSystem::CFileSystem(void)
{
	mLoadKind = LOADKIND_FOLDER;
	mGamePath = NULL;
	mLoadDone = FALSE;
}

BOOL CFileSystem::LoadGameInternal()
{
	switch (mLoadKind)
	{
	case LOADKIND_FOLDER:
		{
			DWORD ft = GetFileAttributes(mGamePath);

			if (ft & FILE_ATTRIBUTE_DIRECTORY)
				return TRUE;
		}
	case LOADKIND_PAK:
		// todo
		break;
	}

	return FALSE;
}

BOOL CFileSystem::LoadGame(LPSTR gamePath, UCHAR loadKind)
{
	if (!gamePath)
		return FALSE;

	if (gamePath == mGamePath && mLoadDone)
		return TRUE;

	mLoadKind = loadKind;
	SAFE_DELETE(mGamePath);
	UINT strSize = strlen(gamePath)+1;
	mGamePath = new CHAR[strSize];
	strcpy_s(mGamePath, strSize, gamePath);

	BOOL ok = LoadGameInternal();
	mLoadDone = ok;

	return ok;
}

VOID* CFileSystem::GetResource(UCHAR kind, LPSTR resName)
{
	if (!mLoadDone)
		return NULL;

	switch (kind)
	{
	case RESOURCEKIND_META:
		resName = RESOURCE_META;
		break;
	case RESOURCEKIND_SCRIPT:
		resName = RESOURCE_SCRIPT;
		break;
	default:
		if (!resName)
			return NULL;
		break;
	}

	UCHAR* data = NULL;

	switch (mLoadKind)
	{
	case LOADKIND_FOLDER:
		{
			static CHAR path[MAX_PATH] = {0};
			sprintf_s(path, MAX_PATH, "%s\\%s", mGamePath, resName);

			FILE* fp = NULL;
			fopen_s(&fp, path, "rb");

			if (!fp)
				return NULL;

			fseek(fp, 0, SEEK_END);
			DWORD fileSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			data = (UCHAR*)malloc(fileSize+1);
			fread((UCHAR*)data, 1, fileSize, fp);
			data[fileSize] = NULL;
			fclose(fp);
		}
		break;
	case LOADKIND_PAK:
		// todo use memory mapping technique
		break;
	}

	return (VOID*)data;
}

VOID CFileSystem::FreeResource(VOID* data)
{
	switch (mLoadKind)
	{
	case LOADKIND_FOLDER:
		if (data) free(data);
		break;
	case LOADKIND_PAK:
		// todo: unmap file
		break;
	}
}

VOID CFileSystem::Release(void)
{
	SAFE_DELETE(mGamePath);
}