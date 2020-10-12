#include "stdafx.h"

#include "FileSystem.h"

#include <cstdio>
#include <cstdlib>

CFileSystem::CFileSystem(void)
{
    mGamePath = "";
    mLoadDone = FALSE;
}

auto CFileSystem::LoadGameInternal() const -> bool
{
    const auto ft = GetFileAttributesA(mGamePath);
    return ft & FILE_ATTRIBUTE_DIRECTORY;
}

auto CFileSystem::FixName(LPCSTR* resName) const -> void
{
    static CHAR fixedTmpName[MAX_PATH] = {0};

    if (!resName)
        return;

    ZeroMemory(fixedTmpName, MAX_PATH);

    auto p = *resName;
    unsigned int i = 0;
    while (*p != 0)
    {
        if (*p == '/')
            fixedTmpName[i] = '\\';
        else fixedTmpName[i] = *p;

        p++;
        i++;
    }

    *resName = fixedTmpName;
}

auto CFileSystem::LoadGame(LPSTR gamePath) -> bool
{
    if (!gamePath)
        return FALSE;

    auto p = gamePath;
    unsigned int validPathI = 0;

    while (*p != 0)
    {
        if (!isspace(*p))
            validPathI++;

        ++p;
    }

    if (!validPathI)
    {
        // path is empty, default to data
        gamePath = "data";
    }

    if (gamePath == mGamePath && mLoadDone)
        return TRUE;

    p = gamePath;
    while (!isspace(*p) && *p != 0)
        p++;

    if (*p != 0)
        *p = 0;

    if (mLoadDone)
        SAFE_DELETE(mGamePath);

    const auto strSize = static_cast<ULONG>(strlen(gamePath)) + 1;
    mGamePath = new CHAR[strSize];
    strcpy_s(mGamePath, strSize, gamePath);

    const auto ok = LoadGameInternal();
    mLoadDone = ok;

    return ok;
}

auto CFileSystem::GetResource(LPCSTR resName/*=NULL*/) const -> FDATA
{
    FDATA res = {nullptr, 0L};

    const auto fp = OpenResource(resName);

    UCHAR* data = nullptr;
    unsigned int size = 0L;

    if (!fp)
        return res;

    fseek(fp, 0, SEEK_END);
    const DWORD fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    data = static_cast<UCHAR*>(neon_malloc(fileSize + 1));
    fread(static_cast<UCHAR*>(data), 1, fileSize, fp);
    data[fileSize] = NULL;
    fclose(fp);

    size = fileSize;

    res.data = data;
    res.size = size;

    return res;
}

void CFileSystem::SaveResource(LPCSTR data, UINT64 size) const
{
    FILE* fp = nullptr;
    fopen_s(&fp, CString::Format("%s\\%s", mGamePath, RESOURCE_UDATA).Str(), "wb");

    fwrite(data, size, 1, fp);
    fclose(fp);
}

auto CFileSystem::OpenResource(LPCSTR resName /*= NULL*/) const -> FILE*
{
    if (!mLoadDone)
        return nullptr;

    FixName(&resName);

    FILE* fp = nullptr;
    fopen_s(&fp, CString::Format("%s\\%s", mGamePath, resName).Str(), "rb");

    return fp;
}

void CFileSystem::CloseResource(FILE* handle)
{
    if (handle)
        fclose(handle);
}

auto CFileSystem::ResourcePath(LPCSTR resName /*= NULL*/) const -> LPCSTR
{
    if (!mLoadDone)
        return nullptr;

    FixName(&resName);

    static CHAR path[MAX_PATH] = {0};
    sprintf_s(path, MAX_PATH, "%s\\%s", mGamePath, resName);
    return path;
}

auto CFileSystem::GetCanonicalGamePath() const -> LPCSTR
{
    static char path[MAX_PATH] = {0};
    LPCSTR p = mGamePath;
    auto s = path;

    while (*p != NULL)
    {
        if (*p == '\\') *s++ = '/';
        else *s++ = *p;
        ++p;
    }

    return path;
}

auto CFileSystem::Exists(LPCSTR resName) -> bool
{
    const auto res = OpenResource(resName);

    if (res)
    {
        CloseResource(res);
        return TRUE;
    }

    return FALSE;
}

void CFileSystem::FreeResource(LPVOID data)
{
    SAFE_FREE(data);
}

void CFileSystem::Release(void)
{
    if (mLoadDone)
        SAFE_DELETE(mGamePath);
}
