#include "stdafx.h"

#include "FileSystem.h"

#include <cstdio>
#include <cstdlib>

CFileSystem::CFileSystem(VOID)
{
	mGamePath = "";
	mLoadDone = FALSE;
}

BOOL CFileSystem::LoadGameInternal()
{
	DWORD ft = GetFileAttributesA(mGamePath);
	return (ft & FILE_ATTRIBUTE_DIRECTORY);
}

VOID CFileSystem::FixName(UCHAR kind, LPCSTR* resName)
{
	static CHAR fixedTmpName[MAX_PATH] = { 0 };

	if (!resName)
		return;

    switch (kind)
    {
		case RESOURCEKIND_UDATA:
			*resName = RESOURCE_UDATA;
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

BOOL CFileSystem::LoadGame(LPSTR gamePath)
{
	if (!gamePath)
		return FALSE;

	LPSTR p = gamePath;
	UINT vdD = 0;

	while (*p != 0)
	{
		if (!isspace(*p))
			vdD++;

		++p;
	}

	if (!vdD)
	{
		// path is empty, default to data
		gamePath = "data";
	}

	if (gamePath == mGamePath && mLoadDone)
		return TRUE;

	p = gamePath;
	while (!isspace(*p) && *p != 0)
		p++;

	if (*p!=0)
		*p=0;

	if (mLoadDone)
		SAFE_DELETE(mGamePath);

	ULONG strSize = (ULONG)strlen(gamePath)+1;
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

	res.data = data;
	res.size = size;

	return res;
}

VOID CFileSystem::SaveResource(UCHAR kind, LPCSTR data, UINT64 size)
{
	if (kind != RESOURCEKIND_UDATA)
		return;

	FILE* fp = NULL;
	fopen_s(&fp, CString::Format("%s\\%s", mGamePath, RESOURCE_UDATA).Str(), "wb");

	fwrite(data, size, 1, fp);
	fclose(fp);
}

FILE* CFileSystem::OpenResource(UCHAR kind, LPCSTR resName /*= NULL*/)
{
    if (!mLoadDone)
        return NULL;

    FixName(kind, &resName);

	FILE* fp = NULL;
	fopen_s(&fp, CString::Format("%s\\%s", mGamePath, resName).Str(), "rb");

	return fp;
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

	static CHAR path[MAX_PATH] = { 0 };
	sprintf_s(path, MAX_PATH, "%s\\%s", mGamePath, resName);
	return path;
}

LPCSTR CFileSystem::GetCanonicalGamePath()
{
	static char path[MAX_PATH] = { 0 };
	LPCSTR p = mGamePath;
	LPSTR s = path;

	while (*p != NULL)
	{
		if (*p == '\\') *s++ = '/';
		else			*s++ = *p;
		++p;
	}

	return path;
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

VOID CFileSystem::FreeResource(LPVOID data)
{
	SAFE_FREE(data);
}

VOID CFileSystem::Release(VOID)
{
	if (mLoadDone)
		SAFE_DELETE(mGamePath);
}
