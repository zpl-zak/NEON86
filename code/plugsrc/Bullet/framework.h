#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <vector>
#include <btBulletDynamicsCommon.h>

extern btDefaultCollisionConfiguration* collisionConfiguration;
extern btCollisionDispatcher* dispatcher;
extern btBroadphaseInterface* overlappingPairCache;
extern btSequentialImpulseConstraintSolver* solver;
extern btDiscreteDynamicsWorld* world;

extern std::vector<btRigidBody*> bodies;
