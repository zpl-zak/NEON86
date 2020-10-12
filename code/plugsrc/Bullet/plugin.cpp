#include "pch.h"
#include "factory.h"

btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
btDiscreteDynamicsWorld* world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver,
                                                             collisionConfiguration);

size_t bodyIndex = 0;
std::unordered_map<size_t, btRigidBody*> bodies;

static auto getMatrix(const btTransform& tr) -> D3DXMATRIX
{
    FLOAT m[16];
    tr.getOpenGLMatrix(m);
    return *reinterpret_cast<D3DXMATRIX*>(m);
}

static auto bullet_update(lua_State* L) -> INT
{
    world->stepSimulation(1 / 60.F, 1);
    return 0;
}

static auto bullet_update_body(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    auto* body = bodies[index];
    body->integrateVelocities(1 / 60.0F);
    return 0;
}

static auto bullet_destroy(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    auto* body = bodies[index];
    world->removeRigidBody(body);
    auto* const shape = body->getCollisionShape();
    auto* const motion = body->getMotionState();
    delete body;
    delete motion;
    delete shape;
    bodies.erase(index);
    return 0;
}

static auto bullet_set_restitution(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto res = static_cast<FLOAT>(luaL_checknumber(L, 2));
    auto* body = bodies[index];
    body->setRestitution(res);
    return 0;
}

static auto bullet_set_collision_mask(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto mask = static_cast<int>(luaL_checkinteger(L, 2));
    auto* body = bodies[index];
    body->getBroadphaseProxy()->m_collisionFilterGroup = mask;
    return 0;
}

static auto bullet_set_mass(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto mass = static_cast<FLOAT>(luaL_checknumber(L, 2));
    auto* body = bodies[index];
    btVector3 localInertia;
    body->getCollisionShape()->calculateLocalInertia(mass, localInertia);
    body->setMassProps(mass, localInertia);
    return 0;
}

static auto bullet_set_gravity(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto gravity = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    auto* body = bodies[index];
    body->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
    return 0;
}

static auto bullet_add_force(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto force = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    auto* body = bodies[index];
    body->applyCentralForce(btVector3(force.x, force.y, force.z));
    return 0;
}

static auto bullet_set_velocity(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto force = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    auto* body = bodies[index];
    body->setLinearVelocity(btVector3(force.x, force.y, force.z));
    return 0;
}

static auto bullet_set_push_velocity(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto force = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    auto* body = bodies[index];
    body->setPushVelocity(btVector3(force.x, force.y, force.z));
    return 0;
}

static auto bullet_get_velocity(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    auto* const body = bodies[index];
    const auto bVel = body->getLinearVelocity();
    *vector4_ctor(L) = D3DXVECTOR4(bVel.x(), bVel.y(), bVel.z(), bVel.w());
    return 1;
}

static auto bullet_set_friction(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto friction = static_cast<float>(luaL_checknumber(L, 2));
    auto* body = bodies[index];
    body->setFriction(friction);
    return 0;
}

static auto bullet_set_damping(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto linear = static_cast<float>(luaL_checknumber(L, 2));
    const auto angular = static_cast<float>(luaL_checknumber(L, 3));
    auto* body = bodies[index];
    body->setDamping(linear, angular);
    return 0;
}

static auto bullet_add_impulse(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto force = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    auto* body = bodies[index];
    body->applyCentralImpulse(btVector3(force.x, force.y, force.z));
    return 0;
}

static auto bullet_set_angular_factor(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto factor = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    auto* body = bodies[index];
    body->setAngularFactor(btVector3(factor.x, factor.y, factor.z));
    return 0;
}

static auto bullet_set_activation_state(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto state = static_cast<int>(luaL_checkinteger(L, 2));
    auto* const body = bodies[index];
    body->setActivationState(state);
    return 0;
}

static auto bullet_set_linear_factor(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    const auto factor = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    auto* body = bodies[index];
    body->setLinearFactor(btVector3(factor.x, factor.y, factor.z));
    return 0;
}

static auto bullet_get_world_transform(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    auto* body = bodies[index];
    btTransform tr;

    if (body->getMotionState() != nullptr)
    {
        body->getMotionState()->getWorldTransform(tr);
    }
    else
    {
        tr = body->getWorldTransform();
    }

    matrix_new(L);
    auto* mat = static_cast<D3DXMATRIX*>(luaL_testudata(L, -1, L_MATRIX));
    *mat = getMatrix(tr);
    return 1;
}

static auto bullet_set_world_transform(lua_State* L) -> INT
{
    const auto index = static_cast<size_t>(luaL_checkinteger(L, 1));
    auto mat = *static_cast<D3DXMATRIX*>(luaL_checkudata(L, 2, L_MATRIX));
    auto* body = bodies[index];
    btTransform tr;
    tr.setIdentity();
    tr.setFromOpenGLMatrix(&mat[0]);
    body->setWorldTransform(tr);
    return 0;
}

static auto bullet_world_set_gravity(lua_State* L) -> INT
{
    const auto gravity = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
    return 0;
}

static auto bullet_world_ray_test(lua_State* L) -> INT
{
    auto start = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 1, L_VECTOR));
    auto end = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    auto collisionMask = -1;

    if (lua_gettop(L) > 2)
    {
        collisionMask = static_cast<int>(luaL_checkinteger(L, 3));
    }

    auto bStart = btVector3(start.x, start.y, start.z);
    auto bEnd = btVector3(end.x, end.y, end.z);

    btCollisionWorld::ClosestRayResultCallback ray(bStart, bEnd);

    if (collisionMask != -1)
    {
        ray.m_collisionFilterMask = collisionMask;
    }

    world->rayTest(bStart, bEnd, ray);

    if (ray.hasHit())
    {
        lua_pushinteger(L, ray.m_collisionObject->getUserIndex());
        *vector4_ctor(L) = D3DXVECTOR4(ray.m_hitPointWorld.x(), ray.m_hitPointWorld.y(), ray.m_hitPointWorld.z(),
                                       ray.m_hitPointWorld.w());
        *vector4_ctor(L) = D3DXVECTOR4(ray.m_hitNormalWorld.x(), ray.m_hitNormalWorld.y(), ray.m_hitNormalWorld.z(),
                                       ray.m_hitNormalWorld.w());
        lua_pushnumber(L, ray.m_closestHitFraction);
        return 4;
    }
    lua_pushnil(L);
    return 1;

    lua_pushnil(L);
    return 1;
}

static const luaL_Reg bullet[] = {
    {"update", bullet_update},
    {"destroy", bullet_destroy},
    {"setWorldGravity", bullet_world_set_gravity},
    {"rayTest", bullet_world_ray_test},

    {"setRestitution", bullet_set_restitution},
    {"integrate", bullet_update_body},
    {"setCollisionMask", bullet_set_collision_mask},
    {"setMass", bullet_set_mass},
    {"setFriction", bullet_set_friction},
    {"setDamping", bullet_set_damping},
    {"setGravity", bullet_set_gravity},
    {"setVelocity", bullet_set_velocity},
    {"setPushVelocity", bullet_set_push_velocity},
    {"getVelocity", bullet_get_velocity},
    {"setLinearFactor", bullet_set_linear_factor},
    {"setAngularFactor", bullet_set_angular_factor},
    {"addForce", bullet_add_force},
    {"addImpulse", bullet_add_impulse},
    {"setActivationState", bullet_set_activation_state},
    {"getWorldTransform", bullet_get_world_transform},
    {"setWorldTransform", bullet_set_world_transform},

    {"createPlane", bullet_body_create_plane},
    {"createSphere", bullet_body_create_sphere},
    {"createCapsule", bullet_body_create_capsule},
    {"createBox", bullet_body_create_box},
    {"createMesh", bullet_body_create_static_cols_mesh},
    {"createMeshFromPart", bullet_body_create_static_cols},
    ENDF
};

extern "C" auto PLUGIN_API luaopen_bullet(lua_State* L) -> INT
{
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
