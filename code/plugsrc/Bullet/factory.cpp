#include "pch.h"
#include "factory.h"

void bullet_body_create_generic(lua_State* L, btCollisionShape* shape, FLOAT mass, const btTransform& tr,
                                btVector3 inertia = btVector3(0, 0, 0))
{
    auto* motion = new btDefaultMotionState(tr);
    const btRigidBody::btRigidBodyConstructionInfo ci(mass, motion, shape, inertia);
    auto* body = new btRigidBody(ci);
    const auto index = bodyIndex++;
    shape->setUserIndex(static_cast<int>(index));
    body->setUserIndex(static_cast<int>(index));
    bodies[index] = body;
    world->addRigidBody(body);
    lua_pushinteger(L, index);
}

auto bullet_body_create_generic_wrapper(lua_State* L, const D3DXMATRIX& mat, btCollisionShape* shape, float mass=0.0f) -> INT
{
    btTransform tr;
    tr.setIdentity();
    tr.setFromOpenGLMatrix(&mat[0]);
    btVector3 localInertia;

    if (mass > 0.0f)
    {
        shape->calculateLocalInertia(mass, localInertia);    
    }
    
    bullet_body_create_generic(L, shape, mass, tr, localInertia);
    return 1;
}

auto bullet_body_create_plane(lua_State* L) -> INT
{
    const auto mat = *static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_VECTOR));
    const auto plane = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));

    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(btVector3(mat._31, mat._32, mat._33));
    btCollisionShape* shape = new btStaticPlaneShape(btVector3(plane.x, plane.y, plane.z), plane.w);
    bullet_body_create_generic(L, shape, 0.0F, tr);
    return 1;
}

static void bullet_populate_mesh_from_part(btTriangleIndexVertexArray* mesh, CFaceGroup* fg)
{
    auto* meshData = new btTriangleMesh(false, false);

    for (UINT i = 0; i < fg->GetNumVertices(); i += 3)
    {
        auto v0 = *(fg->GetVertices() + i);
        auto v1 = *(fg->GetVertices() + i + 1);
        auto v2 = *(fg->GetVertices() + i + 2);
        auto p0 = btVector3(v0.x, v0.y, v0.z);
        auto p1 = btVector3(v1.x, v1.y, v1.z);
        auto p2 = btVector3(v2.x, v2.y, v2.z);

        meshData->addTriangle(p0, p1, p2);
    }

    for (UINT i = 0; i < fg->GetNumIndices(); i += 3)
    {
        auto i0 = *(fg->GetIndices() + i);
        auto i1 = *(fg->GetIndices() + i + 1);
        auto i2 = *(fg->GetIndices() + i + 2);

        meshData->addTriangleIndices(i0, i1, i2);
    }

    mesh->addIndexedMesh(meshData->getIndexedMeshArray().at(0), PHY_SHORT);
}

static void bullet_body_create_static_cols_generic(lua_State* L, btTriangleIndexVertexArray* mesh,
                                                   const D3DXMATRIX& mat)
{
    btCollisionShape* shape = new btBvhTriangleMeshShape(mesh, true);
    bullet_body_create_generic_wrapper(L, mat, shape, 0.0F);
}

auto bullet_body_create_static_cols(lua_State* L) -> INT
{
    const auto mat = *static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    auto* const fg = *static_cast<CFaceGroup**>(luaL_checkudata(L, 2, L_FACEGROUP));

    auto* mesh = new btTriangleIndexVertexArray();
    bullet_populate_mesh_from_part(mesh, fg);

    bullet_body_create_static_cols_generic(L, mesh, mat);
    return 1;
}

auto bullet_body_create_static_cols_mesh(lua_State* L) -> INT
{
    const auto mat = *static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    auto* const m = *static_cast<CMesh**>(luaL_checkudata(L, 2, L_MESH));

    auto* mesh = new btTriangleIndexVertexArray();

    for (UINT i = 0; i < m->GetNumFGroups(); i++)
    {
        auto* const fg = m->GetFGroupData()[i];
        bullet_populate_mesh_from_part(mesh, fg);
    }

    bullet_body_create_static_cols_generic(L, mesh, mat);
    return 1;
}

auto bullet_body_create_sphere(lua_State* L) -> INT
{
    auto mat = *static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto radius = static_cast<FLOAT>(luaL_checknumber(L, 2));
    const auto mass = static_cast<FLOAT>(luaL_checknumber(L, 3));

    bullet_body_create_generic_wrapper(L, mat, new btSphereShape(radius), mass);
    return 1;
}

auto bullet_body_create_box(lua_State* L) -> INT
{
    auto mat = *static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto halfExtents = *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2, L_VECTOR));
    const auto mass = static_cast<FLOAT>(luaL_checknumber(L, 3));
    bullet_body_create_generic_wrapper(L, mat, new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z)), mass);
    return 1;
}

auto bullet_body_create_capsule(lua_State* L) -> INT
{
    auto mat = *static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    const auto radius = static_cast<FLOAT>(luaL_checknumber(L, 2));
    const auto height = static_cast<FLOAT>(luaL_checknumber(L, 3));
    const auto mass = static_cast<FLOAT>(luaL_checknumber(L, 4));
    bullet_body_create_generic_wrapper(L, mat, new btCapsuleShape(radius, height), mass);
    return 1;
}
