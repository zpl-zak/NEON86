#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#ifdef PLUGIN_EXPORTS
#define PLUGIN_API __declspec(dllexport)
#else
#define PLUGIN_API __declspec(dllimport)
#endif

#define SAFE_DELETE(memory) if(memory) { delete memory; memory = NULL; }
#define SAFE_DELETE_ARRAY(memory) if(memory) { delete[] memory; memory = NULL; }
#define SAFE_FREE(memory) if(memory) { neon_free(memory); memory = NULL; }
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = NULL; } }
#define EXPAND_VEC(q) { q.x, q.y, q.z }
#define EXPAND_VECX(q) { q.x, q.y, q.z, 1.0f }
#define EXPAND_VEC4(q) { q.x, q.y, q.z, q.w }
#define EXPAND_COL4(q) { q.r, q.g, q.b, q.a }
#define EXPAND_COLX(q) { q.r, q.g, q.b, 1.0f }

ENGINE_API extern auto GetTime() -> float;
ENGINE_API extern auto ScaleBetween(float x, float a, float b, float na, float nb) -> float;

extern UINT64 gMemUsed, gMemUsedLua, gMemPeak, gResourceCount;
extern void neon_mempeak_update();
extern ENGINE_API auto neon_malloc(size_t size) -> LPVOID;
extern ENGINE_API auto neon_realloc(LPVOID mem, size_t newSize) -> LPVOID;
extern ENGINE_API void neon_free(LPVOID mem);

/// panic handling
#define NEON_PANIC_FN(name) void name(HWND window, LPCSTR text, LPCSTR caption, DWORD style);
typedef NEON_PANIC_FN(neon_panic_ptr);
extern neon_panic_ptr* gPanicHandler;
extern void HandlePanic(HWND window, LPCSTR text, LPCSTR caption, DWORD style);

extern void PushLog(LPCSTR msg, bool noHist = FALSE);

extern auto b64_decode(const char* in, unsigned char* out, size_t outlen) -> int;
extern auto b64_encode(const unsigned char* in, size_t len) -> char*;

/// zpl
#include "zpl_macros.h"

/// lua
#include "lua_macros.h"

#define NOMINMAX
#include <cstdio>

/// helpers
template <typename T>
class ENGINE_API CArray
{
public:
    typedef T* iterator;

    CArray()
    {
        mCapacity = 4;
        mCount = 0;
        mData = static_cast<T*>(neon_malloc(mCapacity * sizeof(T)));  // NOLINT(bugprone-sizeof-expression)
        mIsOwned = TRUE;
    }

    CArray(const CArray<T>& rhs)
    {
        mData = rhs.mData;
        mCount = rhs.mCount;
        mCapacity = rhs.mCapacity;
        mIsOwned = FALSE;
    }

    ~CArray()
    {
        Release();
    }

    auto begin() const -> iterator { return &mData[0]; }
    auto end() const -> iterator { return &mData[mCount]; }

    void Release()
    {
        if (mIsOwned)
        {
            SAFE_FREE(mData);
            mCapacity = 4;
            mCount = 0;
        }
    }

    auto Push(T elem) -> HRESULT
    {
        if (!mData)
            mData = static_cast<T*>(neon_realloc(mData, mCapacity * sizeof(T)));

        if (mCount >= mCapacity)
        {
            mCapacity += 4;
            mData = static_cast<T*>(neon_realloc(mData, mCapacity * sizeof(T)));

            if (!mData)
            {
                return E_OUTOFMEMORY;
            }
        }

        mData[mCount++] = elem;
        return ERROR_SUCCESS;
    }

    auto Find(LPCSTR name) const -> T
    {
        for (unsigned int i = 0; i < mCount; i++)
        {
            if (!strcmp(name, mData[i]->GetName().Str()))
                return mData[i];
        }

        return NULL;
    }

    void RemoveByValue(T val)
    {
        unsigned int idx = -1;

        for (unsigned int i = 0; i < mCount; i++)
        {
            if (mData[idx] == val)
            {
                idx = i;
                break;
            }
        }

        if (idx != -1)
            RemoveByIndex(idx);
    }

    auto RemoveByIndex(unsigned int idx) -> T
    {
        T* ptr = mData + idx;
        ::memmove(mData + idx, mData + idx + 1, (mCount - idx - 1) * sizeof(T));
        mCount--;

        return *ptr;
    }

    void Clear() { mCount = 0; }

    auto GetCount() const -> unsigned int { return mCount; }
    auto GetCapacity() const -> unsigned int { return mCapacity; }

    auto operator[](unsigned int index) const -> T { return mData[index]; }
    auto GetData() const -> T* { return mData; }

private:
    T* mData;
    unsigned int mCapacity;
    unsigned int mCount;
    bool mIsOwned;
};

class ENGINE_API CString
{
public:
    CString()
    {
        mStr = nullptr;
        mSize = 0;
    };

    CString(LPCSTR str)
    {
        Assign(str);
    }

    CString(const CString& rhs)
    {
        Assign(rhs.mStr);
    }

    CString(CString* s1, CString s2)
    {
        Concatenate(s1->Str(), s2.Str());
    }

    ~CString()
    {
        SAFE_DELETE_ARRAY(mStr);
    }

    auto Length() const -> SSIZE_T
    {
        return mSize;
    }

    auto Str() const -> LPCSTR
    {
        return mStr;
    }

    auto Find(LPCSTR str) const -> LPCSTR
    {
        const LPCSTR out = strstr(mStr, str);
        return out ? out : nullptr;
    }

    auto operator==(LPCSTR str) const -> bool
    {
        return strcmp(mStr, str) == 0;
    }

    auto operator!=(LPCSTR str) const -> bool
    {
        return !(mStr == str);
    }

    void operator=(LPCSTR str)
    {
        Assign(str);
    }

    void operator=(CString str)
    {
        Assign(str.Str());
    }

    auto operator+=(CString str) -> CString
    {
        return CString(this, str);
    }

    static auto Format(LPCSTR fmt, ...) -> CString
    {
        CHAR buf[4096] = {0};
        va_list va;
        va_start(va, fmt);
        vsnprintf_s(buf, 4096, fmt, va);
        va_end(va);

        return CString(buf);
    }

private:
    LPSTR mStr;
    SSIZE_T mSize;

    void Assign(LPCSTR str)
    {
        mSize = strlen(str);
        mStr = new CHAR[mSize + 1];
        strcpy_s(mStr, mSize + 1, str);
    }

    void Concatenate(LPCSTR s1, LPCSTR s2)
    {
        mSize = strlen(s1) + strlen(s2);
        mStr = new CHAR[mSize + 1];
        strcat_s(mStr, mSize + 1, s1);
        strcat_s(mStr, mSize + 1, s2);
    }
};

class ENGINE_API NoCopy
{
public:
    NoCopy(){}
    NoCopy(const NoCopy&) = delete;
};

class ENGINE_API NoAssign
{
public:
    NoAssign(){}
    NoAssign& operator=(const NoAssign&) = delete;
};

class ENGINE_API NoCopyAssign: NoCopy, NoAssign {

};
