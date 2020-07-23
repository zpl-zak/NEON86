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

ENGINE_API extern FLOAT GetTime();
ENGINE_API extern FLOAT ScaleBetween(FLOAT x, FLOAT a, FLOAT b, FLOAT na, FLOAT nb);

extern UINT64 gMemUsed, gMemUsedLua, gMemPeak, gResourceCount;
extern VOID neon_mempeak_update();
extern LPVOID neon_malloc(size_t size);
extern LPVOID neon_realloc(LPVOID mem, size_t newSize);
extern VOID neon_free(LPVOID mem);

/// panic handling
#define NEON_PANIC_FN(name) VOID name(HWND window, LPCSTR text, LPCSTR caption, DWORD style);
typedef NEON_PANIC_FN(neon_panic_ptr);
extern neon_panic_ptr* gPanicHandler;
extern void HandlePanic(HWND window, LPCSTR text, LPCSTR caption, DWORD style);

/// zpl
#include "zpl_macros.h"

/// lua
#include "lua_macros.h"


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
        mData = (T*)neon_malloc(mCapacity * sizeof(T));
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

    iterator begin() const { return &mData[0]; }
    iterator end() const { return &mData[mCount]; }

    inline VOID Release()
    {
        if (mIsOwned)
        {
            SAFE_FREE(mData);
            mCapacity = 4;
            mCount = 0;
        }
    }

    inline HRESULT Push(T elem)
    {
        if (!mData)
            mData = (T*)neon_realloc(mData, mCapacity * sizeof(T));

        if (mCount >= mCapacity)
        {
            mCapacity += 4;
            mData = (T*)neon_realloc(mData, mCapacity * sizeof(T));

            if (!mData)
            {
                return E_OUTOFMEMORY;
            }
        }

        mData[mCount++] = elem;
        return ERROR_SUCCESS;
    }

    inline T Find(LPCSTR name) const
    {
        for (UINT i = 0; i < mCount; i++)
        {
            if (!strcmp(name, mData[i]->GetName().Str()))
                return mData[i];
        }

        return NULL;
    }

    inline T RemoveByIndex(UINT idx) {
        T* ptr = (mData + idx);
        ::memmove(mData + idx, mData + idx + 1, (mCount - idx - 1) * sizeof(T));
        mCount--;

        return *ptr;
    }

    inline VOID Clear() { mCount = 0; }

    inline UINT GetCount() const { return mCount; }
    inline UINT GetCapacity() const { return mCapacity; }

    inline T operator[] (UINT index) const { return mData[index]; }
    inline T* GetData() const { return mData; }

private:
    T* mData;
    UINT mCapacity;
    UINT mCount;
    BOOL mIsOwned;
};

#include <string>

class ENGINE_API CString {
public:
    CString() {
        mStr = NULL;
        mSize = 0;
    };
    CString(LPCSTR str) {
        Assign(str);
    }
    CString(LPSTR str) {
        Assign((LPCSTR)str);
    }
    CString(std::string str) {
        Assign(str.c_str());
    }
    CString(const CString& rhs) {
        Assign(rhs.mStr);
    }

    ~CString() {
        SAFE_DELETE_ARRAY(mStr);
    }

    SSIZE_T Length() {
        return mSize;
    }

    LPCSTR Str() {
        return mStr;
    }

    std::string SStr() {
        return std::string(mStr);
    }

    BOOL operator== (LPCSTR str) {
        return (::strcmp(mStr, str) == 0);
    }

    BOOL operator!= (LPCSTR str) {
        return !(mStr == str);
    }

    VOID operator= (LPCSTR str) {
        Assign(str);
    }

    VOID operator= (CString str) {
        Assign(str.Str());
    }
private:
    LPSTR mStr;
    SSIZE_T mSize;

    VOID Assign(LPCSTR str) {
        mSize = ::strlen(str);
        mStr = new CHAR[mSize + 1];
        ::strcpy_s(mStr, mSize + 1, str);
    }
};
