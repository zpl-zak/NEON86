#pragma once

struct lua_State;

extern INT bullet_body_create_plane(lua_State* L);
extern INT bullet_body_create_sphere(lua_State* L);
extern INT bullet_body_create_capsule(lua_State* L);
extern INT bullet_body_create_static_cols(lua_State* L);
