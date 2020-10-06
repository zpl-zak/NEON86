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

static INT bullet_set_collision_mask(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    int mask = (int)luaL_checkinteger(L, 2);
    btRigidBody* body = bodies[index];
    body->getBroadphaseProxy()->m_collisionFilterGroup = mask;
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

static INT bullet_set_angular_factor(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    D3DXVECTOR4 factor = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
    btRigidBody* body = bodies[index];
    body->setAngularFactor(btVector3(factor.x, factor.y, factor.z));
    return 0;
}

static INT bullet_set_activation_state(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    int state = (int)luaL_checkinteger(L, 2);
    btRigidBody* body = bodies[index];
    body->setActivationState(state);
    return 0;
}

static INT bullet_set_linear_factor(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    D3DXVECTOR4 factor = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
    btRigidBody* body = bodies[index];
    body->setLinearFactor(btVector3(factor.x, factor.y, factor.z));
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

static INT bullet_set_world_transform(lua_State* L) {
    size_t index = (size_t)luaL_checkinteger(L, 1);
    D3DXMATRIX mat = *(D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);
    btRigidBody* body = bodies[index];
    btTransform tr;
    tr.setIdentity();
    tr.setFromOpenGLMatrix(&mat[0]);
    body->setWorldTransform(tr);
    return 0;
}

static INT bullet_world_set_gravity(lua_State* L) {
    D3DXVECTOR4 gravity = *(D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
    return 0;
}

static INT bullet_world_ray_test(lua_State* L) {
    D3DXVECTOR4 start = *(D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    D3DXVECTOR4 end = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
    int collisionMask = -1;

    if (lua_gettop(L) > 2)
        collisionMask = (int)luaL_checkinteger(L, 3);

    btVector3 bStart = btVector3(start.x, start.y, start.z);
    btVector3 bEnd = btVector3(end.x, end.y, end.z);

    btCollisionWorld::ClosestRayResultCallback ray(bStart, bEnd);

    if (collisionMask != -1)
        ray.m_collisionFilterMask = collisionMask;

    world->rayTest(bStart, bEnd, ray);

    if (ray.hasHit())
    {
        lua_pushinteger(L, ray.m_collisionObject->getUserIndex());
        *vector4_ctor(L) = D3DXVECTOR4(ray.m_hitPointWorld.x(), ray.m_hitPointWorld.y(), ray.m_hitPointWorld.z(), ray.m_hitPointWorld.w());
        *vector4_ctor(L) = D3DXVECTOR4(ray.m_hitNormalWorld.x(), ray.m_hitNormalWorld.y(), ray.m_hitNormalWorld.z(), ray.m_hitNormalWorld.w());
        lua_pushnumber(L, ray.m_closestHitFraction);
        return 4;
    }
    else 
    {
        lua_pushnil(L);
        return 1;
    }
}

static const luaL_Reg bullet[] = {
    { "update", bullet_update },
    { "destroy", bullet_destroy },
    { "setWorldGravity", bullet_world_set_gravity },
    { "rayTest", bullet_world_ray_test },

    { "setRestitution", bullet_set_restitution },
    { "setCollisionMask", bullet_set_collision_mask },
    { "setMass", bullet_set_mass },
    { "setFriction", bullet_set_friction },
    { "setDamping", bullet_set_damping },
    { "setGravity", bullet_set_gravity },
    { "setVelocity", bullet_set_velocity },
    { "setLinearFactor", bullet_set_linear_factor },
    { "setAngularFactor", bullet_set_angular_factor },
    { "addForce", bullet_add_force },
    { "addImpulse", bullet_add_impulse },
    { "setActivationState", bullet_set_activation_state },
    { "getWorldTransform", bullet_get_world_transform },
    { "setWorldTransform", bullet_set_world_transform },

    { "createPlane", bullet_body_create_plane },
    { "createSphere", bullet_body_create_sphere },
    { "createCapsule", bullet_body_create_capsule },
    { "createBox", bullet_body_create_box },
    { "createMesh", bullet_body_create_static_cols_mesh },
    { "createMeshFromPart", bullet_body_create_static_cols },
    ENDF
};

extern "C" INT PLUGIN_API luaopen_bullet(lua_State* L) {
    luaL_newlib(L, bullet);
    world->setGravity(btVector3(0, -10, 0));

    lua_pushnumber(L, ACTIVE_TAG);
    lua_setfield(L, -2, "ACTIVE_TAG");

    lua_pushnumber(L, ISLAND_SLEEPING);
    lua_setfield(L, -2, "ISLAND_SLEEPING");

    lua_pushnumber(L, WANTS_DEACTIVATION);
    lua_setfield(L, -2, "WANTS_DEACTIVATION");

    lua_pushnumber(L, DISABLE_DEACTIVATION);
    lua_setfield(L, -2, "DISABLE_DEACTIVATION");

    lua_pushnumber(L, DISABLE_SIMULATION);
    lua_setfield(L, -2, "DISABLE_SIMULATION");
    return 1;
}
