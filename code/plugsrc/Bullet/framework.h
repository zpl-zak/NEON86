#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <unordered_map>
#include <btBulletDynamicsCommon.h>

extern btDefaultCollisionConfiguration* collisionConfiguration;
extern btCollisionDispatcher* dispatcher;
extern btBroadphaseInterface* overlappingPairCache;
extern btSequentialImpulseConstraintSolver* solver;
extern btDiscreteDynamicsWorld* world;

extern size_t bodyIndex;
extern std::unordered_map<size_t, btRigidBody*> bodies;
