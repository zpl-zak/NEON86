#pragma once

#include "system.h"

struct FDATA
{
    LPVOID data;
    unsigned int size;
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
    CFileSystem(void);
    auto LoadGame(LPSTR gamePath) -> bool;
    auto GetResource(LPCSTR resName = nullptr) const -> FDATA;
    void SaveResource(LPCSTR data, UINT64 size) const;
    auto OpenResource(LPCSTR resName = nullptr) const -> FILE*;
    static void CloseResource(FILE* handle);
    auto ResourcePath(LPCSTR resName = nullptr) const -> LPCSTR;
    auto GetGamePath() const -> LPCSTR { return mGamePath; }
    auto GetCanonicalGamePath() const -> LPCSTR;
    auto Exists(LPCSTR resName) -> bool;
    static void FreeResource(LPVOID data);
    void Release(void);

private:
    LPSTR mGamePath;
    bool mLoadDone;

    auto LoadGameInternal() const -> bool;
    static void FixName(LPCSTR* resName);
};
