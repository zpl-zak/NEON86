#include "pch.h"
#include "factory.h"

btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
btDiscreteDynamicsWorld* world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

std::vector<btRigidBody*> bodies;

static D3DXMATRIX getMatrix(const btTransform& tr) {
    FLOAT m[16];
    tr.getOpenGLMatrix(m);
    return *(D3DXMATRIX*)m;
}

static INT bullet_update(lua_State* L) {
    world->stepSimulation(1 / 60.f, 1);
    return 0;
}

static INT bullet_destroy(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    btRigidBody* body = bodies[index];
    world->removeRigidBody(body);
    btCollisionShape* shape = body->getCollisionShape();
    btMotionState* motion = body->getMotionState();
    delete body;
    delete motion;
    delete shape;
    bodies.erase(bodies.begin() + index);
    return 0;
}

static INT bullet_set_restitution(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    FLOAT res = (FLOAT)luaL_checknumber(L, 2);
    btRigidBody* body = bodies[index];
    body->setRestitution(res);
    return 0;
}

static INT bullet_set_mass(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    FLOAT mass = (FLOAT)luaL_checknumber(L, 2);
    btRigidBody* body = bodies[index];
    btVector3 localInertia;
    body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
    body->setMassProps(mass, localInertia);
    return 0;
}

static INT bullet_set_gravity(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    D3DXVECTOR4 gravity = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
    btRigidBody* body = bodies[index]; 
    body->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
    return 0;
}

static INT bullet_add_force(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    D3DXVECTOR4 force = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
    btRigidBody* body = bodies[index];
    body->applyCentralForce(btVector3(force.x, force.y, force.z));
    return 0;
}

static INT bullet_set_velocity(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    D3DXVECTOR4 force = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
    btRigidBody* body = bodies[index];
    body->setLinearVelocity(btVector3(force.x, force.y, force.z));
    return 0;
}

static INT bullet_set_friction(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    float friction = (float)luaL_checknumber(L, 2);
    btRigidBody* body = bodies[index];
    body->setFriction(friction);
    return 0;
}

static INT bullet_set_damping(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    float linear = (float)luaL_checknumber(L, 2);
    float angular = (float)luaL_checknumber(L, 3);
    btRigidBody* body = bodies[index];
    body->setDamping(linear, angular);
    return 0;
}

static INT bullet_add_impulse(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    D3DXVECTOR4 force = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
    btRigidBody* body = bodies[index];
    body->applyCentralImpulse(btVector3(force.x, force.y, force.z));
    return 0;
}

static INT bullet_disable_sleep(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    btRigidBody* body = bodies[index];
    body->setActivationState(DISABLE_DEACTIVATION);
    return 0;
}

static INT bullet_get_world_transform(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    btRigidBody* body = bodies[index];
    btTransform tr;

    if (body->getMotionState())
        body->getMotionState()->getWorldTransform(tr);
    else
        tr = body->getWorldTransform();

    matrix_new(L);
    D3DXMATRIX* mat = (D3DXMATRIX*)luaL_testudata(L, -1, L_MATRIX);
    *mat = getMatrix(tr);
    return 1;
}

static INT bullet_world_set_gravity(lua_State* L) {
    D3DXVECTOR4 gravity = *(D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
    return 0;
}

static const luaL_Reg bullet[] = {
    { "update", bullet_update },
    { "destroy", bullet_destroy },
    
    { "setRestitution", bullet_set_restitution },
    { "setMass", bullet_set_mass },
    { "setFriction", bullet_set_friction },
    { "setDamping", bullet_set_damping },
    { "setGravity", bullet_set_gravity },
    { "setVelocity", bullet_set_velocity },
    { "addForce", bullet_add_force },
    { "addImpulse", bullet_add_impulse },
    { "disableSleep", bullet_disable_sleep },
    { "setWorldGravity", bullet_world_set_gravity },
    { "getWorldTransform", bullet_get_world_transform },

    { "createPlane", bullet_body_create_plane },
    { "createSphere", bullet_body_create_sphere },
    { "createMesh", bullet_body_create_static_cols },
    ENDF
};

extern "C" INT PLUGIN_API luaopen_bullet(lua_State* L) {
    luaL_newlib(L, bullet);
    world->setGravity(btVector3(0, -10, 0));
    return 1;
}
