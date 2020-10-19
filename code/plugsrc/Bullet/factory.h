#pragma once

struct lua_State;

extern auto bullet_body_create_plane(lua_State* L) -> INT;
extern auto bullet_body_create_sphere(lua_State* L) -> INT;
extern auto bullet_body_create_box(lua_State* L) -> INT;
extern auto bullet_body_create_capsule(lua_State* L) -> INT;
extern auto bullet_body_create_static_cols(lua_State* L) -> INT;
extern auto bullet_body_create_static_cols_mesh(lua_State* L) -> INT;
