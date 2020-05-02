#include "stdafx.h"

#include "FileSystem.h"

#include <cstdio>
#include <cstdlib>

CFileSystem::CFileSystem(VOID)
{
	mLoadKind = LOADKIND_FOLDER;
	mGamePath = "";
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

VOID CFileSystem::FixName(UCHAR kind, LPCSTR* resName)
{
	static CHAR fixedTmpName[MAX_PATH] = { 0 };

	if (!resName)
		return;

    switch (kind)
    {
        case RESOURCEKIND_META:
            *resName = RESOURCE_META;
            break;
        case RESOURCEKIND_SCRIPT:
            *resName = RESOURCE_SCRIPT;
            break;
		default:
			ZeroMemory(fixedTmpName, MAX_PATH);

			LPCSTR p = *resName;
			UINT i = 0;
			while (*p != 0)
			{
				if (*p == '/')
					fixedTmpName[i] = '\\';
				else fixedTmpName[i] = *p;

				p++;
				i++;
			}

			*resName = fixedTmpName;
			break;
    }
}

BOOL CFileSystem::ValidatePath(LPCSTR path, LPCSTR dir)
{
	CHAR buf[MAX_PATH] = { 0 };
    WIN32_FIND_DATA data;

	if (!dir)
	{
		sprintf_s(buf, MAX_PATH, "%s\\*", mGamePath);
	}
	else
		sprintf_s(buf, MAX_PATH, "%s\\%s\\*", mGamePath, dir);
    HANDLE hFind = FindFirstFile(buf, &data);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
			if (!strcmp(data.cFileName, ".") || !strcmp(data.cFileName, ".."))
				continue;

			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (dir)
					sprintf_s(buf, MAX_PATH, "%s\\%s", dir, data.cFileName);
				else
					sprintf_s(buf, MAX_PATH, "%s", data.cFileName);

				if (ValidatePath(path, buf))
				{
					FindClose(hFind);
					return TRUE;
				}
			}

			if (dir)
				sprintf_s(buf, MAX_PATH, "%s\\%s", dir, data.cFileName);
			else
				sprintf_s(buf, MAX_PATH, "%s", data.cFileName);

			if (!strcmp(buf, path))
			{
				FindClose(hFind);
				return TRUE;
			}
        } while (FindNextFile(hFind, &data));
        FindClose(hFind);
    }

	return FALSE;
}

BOOL CFileSystem::LoadGame(LPSTR gamePath, UCHAR loadKind)
{
	if (!gamePath)
		return FALSE;

	if (gamePath == mGamePath && mLoadDone)
		return TRUE;

	LPSTR p = gamePath;
	while (!isspace(*p) && *p != 0)
		p++;

	if (*p!=0)
		*p=0;

	mLoadKind = loadKind;

	if (mLoadDone)
		SAFE_DELETE(mGamePath);

	UINT strSize = strlen(gamePath)+1;
	mGamePath = new CHAR[strSize];
	strcpy_s(mGamePath, strSize, gamePath);

	BOOL ok = LoadGameInternal();
	mLoadDone = ok;

	return ok;
}

FDATA CFileSystem::GetResource(UCHAR kind, LPCSTR resName/*=NULL*/)
{
	FDATA res={NULL, 0L};

	FILE* fp = OpenResource(kind, resName);

	UCHAR* data = NULL;
	UINT size = 0L;

	switch (mLoadKind)
	{
	case LOADKIND_FOLDER:
		{
			if (!fp)
				return res;

			fseek(fp, 0, SEEK_END);
			DWORD fileSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			data = (UCHAR*)neon_malloc(fileSize+1);
			fread((UCHAR*)data, 1, fileSize, fp);
			data[fileSize] = NULL;
			fclose(fp);

			size = fileSize;
		}
		break;
	case LOADKIND_PAK:
		// todo use memory mapping technique
		break;
	}

	res.data = data;
	res.size = size;

	return res;
}

FILE* CFileSystem::OpenResource(UCHAR kind, LPCSTR resName /*= NULL*/)
{
    if (!mLoadDone)
        return NULL;

    FixName(kind, &resName);

	if (!ValidatePath(resName))
		return NULL;

    switch (mLoadKind)
    {
        case LOADKIND_FOLDER:
        {
            static CHAR path[MAX_PATH] = { 0 };
            sprintf_s(path, MAX_PATH, "%s\\%s", mGamePath, resName);

            FILE* fp = NULL;
            fopen_s(&fp, path, "rb");

			return fp;
        }
        break;
        case LOADKIND_PAK:
            // todo use memory mapping technique
            break;
    }

	return NULL;
}

VOID CFileSystem::CloseResource(FILE* handle)
{
	if (handle)
		fclose(handle);
}

LPCSTR CFileSystem::ResourcePath(UCHAR kind, LPCSTR resName /*= NULL*/)
{
    if (!mLoadDone)
        return NULL;

    FixName(kind, &resName);

    switch (mLoadKind)
    {
        case LOADKIND_FOLDER:
        {
            static CHAR path[MAX_PATH] = { 0 };
            sprintf_s(path, MAX_PATH, "%s\\%s", mGamePath, resName);
			return path;
        }
        break;
        case LOADKIND_PAK:
            // todo use memory mapping technique
			// we might also need to expose the file temporarily here
            break;
    }

    return NULL;
}

BOOL CFileSystem::Exists(UCHAR kind, LPCSTR resName)
{
	FILE* res = OpenResource(kind, resName);

	if (res)
	{
		CloseResource(res);
		return TRUE;
	}

    return FALSE;
}

VOID CFileSystem::FreeResource(VOID* data)
{
	switch (mLoadKind)
	{
	case LOADKIND_FOLDER:
		SAFE_FREE(data);
		break;
	case LOADKIND_PAK:
		// todo: unmap file
		break;
	}
}

VOID CFileSystem::Release(VOID)
{
	if (mLoadDone)
		SAFE_DELETE(mGamePath);
}
