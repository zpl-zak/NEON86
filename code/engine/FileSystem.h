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
    auto LoadGame(LPSTR gamePath) -> BOOL;
    auto GetResource(LPCSTR resName = nullptr) const -> FDATA;
    VOID SaveResource(LPCSTR data, UINT64 size) const;
    auto OpenResource(LPCSTR resName = nullptr) const -> FILE*;
    VOID CloseResource(FILE* handle);
    auto ResourcePath(LPCSTR resName = nullptr) const -> LPCSTR;
    auto GetGamePath() const -> LPCSTR { return mGamePath; }
    auto GetCanonicalGamePath() const -> LPCSTR;
    auto Exists(LPCSTR resName) -> BOOL;
    VOID FreeResource(LPVOID data);
    VOID Release(VOID);

private:
    LPSTR mGamePath;
    BOOL mLoadDone;

    auto LoadGameInternal() const -> BOOL;
    VOID FixName(LPCSTR* resName) const;
};
