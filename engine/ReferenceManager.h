#pragma once

#include "system.h"

#include <vector>

class CFaceGroup;
class CMesh;
class CMaterial;
class CLight;
class CTarget;

template<typename T>
class CReference
{
public:
    VOID Release();
    VOID TrackRef(T*);
private:
    T* mSelf;
};

template<typename T>
inline VOID CReference<T>::Release()
{
    mSelf->Release();
}

template<typename T>
class CReferenceContainer
{
public:
    VOID AddRef(CReference<T> m);
    VOID Release();

private:
    std::vector<CReference<T>> mRefs;
};

class CReferenceManager
{
public:
    static VOID Release();

    template<typename T>
    inline static VOID TrackRef(CReference<T>* t) {
        t->TrackRef((T*)t);
    }

    static CReferenceContainer<CFaceGroup> faceGroups;
    static CReferenceContainer<CMaterial> materials;
    static CReferenceContainer<CMesh> meshes;
    static CReferenceContainer<CLight> lights;
    static CReferenceContainer<CTarget> targets;
};

template<typename T>
inline VOID CReferenceContainer<T>::AddRef(CReference<T> m)
{
    mRefs.push_back(m);
}

template<typename T>
inline VOID CReferenceContainer<T>::Release()
{
    for (auto r : mRefs)
        r.Release();

    mRefs.clear();
}

inline VOID CReference<CFaceGroup>::TrackRef(CFaceGroup* self)
{
    mSelf = self;
    CReferenceManager::faceGroups.AddRef(*this);
}

inline VOID CReference<CMaterial>::TrackRef(CMaterial* self)
{
    mSelf = self;
    CReferenceManager::materials.AddRef(*this);
}

inline VOID CReference<CMesh>::TrackRef(CMesh* self)
{
    mSelf = self;
    CReferenceManager::meshes.AddRef(*this);
}

inline VOID CReference<CLight>::TrackRef(CLight* self)
{
    mSelf = self;
    CReferenceManager::lights.AddRef(*this);
}

inline VOID CReference<CTarget>::TrackRef(CTarget* self)
{
    mSelf = self;
    CReferenceManager::targets.AddRef(*this);
}
