#pragma once

#include "system.h"

#include <vector>

class CFaceGroup;
class CMesh;
class CMaterial;

template<typename T>
class CReference
{
public:
    void Release();
    void TrackRef(T*);
private:
    T* mSelf;
};

template<typename T>
inline void CReference<T>::Release()
{
    mSelf->Release();
}

template<typename T>
class CReferenceContainer
{
public:
    void AddRef(CReference<T> m);
    void Release();

private:
    std::vector<CReference<T>> mRefs;
};

class CReferenceManager
{
public:
    static void Release();

    template<typename T>
    inline static void TrackRef(CReference<T>* t) {
        t->TrackRef((T*)t);
    }

    static CReferenceContainer<CFaceGroup> meshes;
    static CReferenceContainer<CMaterial> materials;
    static CReferenceContainer<CMesh> meshGroups;
};

template<typename T>
inline void CReferenceContainer<T>::AddRef(CReference<T> m)
{
    mRefs.push_back(m);
}

template<typename T>
inline void CReferenceContainer<T>::Release()
{
    for (auto r : mRefs)
        r.Release();

    mRefs.clear();
}

inline void CReference<CFaceGroup>::TrackRef(CFaceGroup* self)
{
    mSelf = self;
    CReferenceManager::meshes.AddRef(*this);
}

inline void CReference<CMaterial>::TrackRef(CMaterial* self)
{
    mSelf = self;
    CReferenceManager::materials.AddRef(*this);
}

inline void CReference<CMesh>::TrackRef(CMesh* self)
{
    mSelf = self;
    CReferenceManager::meshGroups.AddRef(*this);
}
