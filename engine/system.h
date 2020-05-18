#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
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

extern FLOAT GetTime();


extern DWORD gMemUsed, gMemUsedLua, gMemPeak, gResourceCount;
extern VOID neon_mempeak_update();
extern LPVOID neon_malloc(DWORD size);
extern LPVOID neon_realloc(LPVOID mem, DWORD newSize);
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
class CArray: public CReferenceCounter
{
public:
    typedef T* iterator;

    CArray():CReferenceCounter()
    {
        mCapacity = 4;
        mCount = 0;
        mData = (T*)neon_malloc(mCapacity * sizeof(T));
    }

    CArray(const CArray<T>& rhs)
    {
        mData = rhs.mData;
        mCount = rhs.mCount;
        mCapacity = rhs.mCapacity;
        AddRef();
    }

    ~CArray()
    {
        Release();
    }

    iterator begin() { return &mData[0]; }
    iterator end() { return &mData[mCount]; }

    inline VOID Release()
    {
        if (DelRef())
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

    inline T Find(LPCSTR name)
    {
        for (UINT i = 0; i < mCount; i++)
        {
            if (!strcmp(name, mData[i]->GetName().C_Str()))
                return mData[i];
        }

        return NULL;
    }

    inline VOID Clear() { mCount = 0; }

    inline UINT GetCount() { return mCount; }
    inline UINT GetCapacity() { return mCapacity; }

    inline T operator[] (UINT index) { return mData[index]; }
    inline T* GetData() { return mData; }

private:
    T* mData;
    UINT mCapacity;
    UINT mCount;
};

#include <vector>
#include <string>

inline std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}
