#include "stdafx.h"
#include "LuaBindings.h"

#include "Engine.h"
#include "VM.h"
#include "Renderer.h"
#include "Input.h"
#include "FileSystem.h"
#include "Sound.h"
#include "Material.h"
#include "FaceGroup.h"
#include "Scene.h"
#include "Font.h"
#include "RenderTarget.h"
#include "Music.h"
#include "Node.h"

#include <lua/lua.hpp>

#include "LuaWrapper.h"

auto luaH_getcomps(lua_State* L, unsigned int offset) -> D3DXVECTOR4
{
    if (luaL_testudata(L, 2 + offset, L_VECTOR))
    {
        return *static_cast<D3DXVECTOR4*>(luaL_checkudata(L, 2 + offset, L_VECTOR));
    }

    const auto x = static_cast<float>(lua_tonumber(L, 2 + offset));
    auto y = static_cast<float>(lua_tonumber(L, 3 + offset));
    auto z = static_cast<float>(lua_tonumber(L, 4 + offset));
    auto w = static_cast<float>(lua_tonumber(L, 5 + offset));

    if (lua_gettop(L) == 2 + offset)
        w = y = z = x;

    if (lua_gettop(L) == 3 + offset)
        w = z = x;

    if (lua_gettop(L) == 4 + offset)
        w = 0;

    return {x, y, z, w};
}

auto luaH_getcolor(lua_State* L) -> DWORD
{
    DWORD color = 0x0;

    if (LuaTestClass(L, L_VECTOR))
    {
        const auto vec = LuaGetInline<D3DXVECTOR4>(L);
        BYTE col[4] = {
            static_cast<BYTE>(vec.w * 0xFF), static_cast<BYTE>(vec.x * 0xFF), static_cast<BYTE>(vec.y * 0xFF),
            static_cast<BYTE>(vec.z * 0xFF)
        };
        color = D3DCOLOR_ARGB(col[0], col[1], col[2], col[3]);
    }
    else if (LuaLength(L) >= 1)
    {
        color = LuaGetInline<DWORD>(L);
    }
    else if (LuaLength(L) >= 3)
    {
        unsigned int r = 0;
        unsigned int g = 0;
        unsigned int b = 0;

        r = LuaGetInline<uint32_t>(L);
        g = LuaGetInline<uint32_t>(L);
        b = LuaGetInline<uint32_t>(L);
        color = D3DCOLOR_ARGB(255, r, g, b);
    }

    return color;
}

auto luaH_getcolorlinear(lua_State* L, unsigned int offset) -> D3DCOLORVALUE
{
    D3DCOLORVALUE color = {0.0F, 0.0F, 0.0F, 1.0F};

    if (luaL_testudata(L, 1 + offset, L_VECTOR))
    {
        color = *static_cast<D3DCOLORVALUE*>(luaL_checkudata(L, 1 + offset, L_VECTOR));
    }
    else if (static_cast<unsigned int>(lua_gettop(L)) == 1 + offset)
    {
        const auto encodedColor = static_cast<DWORD>(luaL_checkinteger(L, 1 + offset));
        BYTE r;
        BYTE g;
        BYTE b;
        BYTE a;

        if (encodedColor <= 0xFFFFFF)
        {
            r = static_cast<BYTE>((encodedColor & 0xFF0000) >> 16);
            g = static_cast<BYTE>((encodedColor & 0x00FF00) >> 8);
            b = static_cast<BYTE>((encodedColor & 0x0000FF) >> 0);
        }
        else
        {
            a = static_cast<BYTE>((encodedColor & 0xFF000000) >> 24);
            r = static_cast<BYTE>((encodedColor & 0x00FF0000) >> 16);
            g = static_cast<BYTE>((encodedColor & 0x0000FF00) >> 8);
            b = static_cast<BYTE>((encodedColor & 0x000000FF) >> 0);
        }

        color = {
            static_cast<float>(r) / 0xFF, static_cast<float>(g) / 0xFF, static_cast<float>(b) / 0xFF,
            static_cast<float>(a) / 0xFF
        };
    }
    else if (static_cast<unsigned int>(lua_gettop(L)) == 3 + offset)
    {
        unsigned int r = 0;
        unsigned int g = 0;
        unsigned int b = 0;

        r = static_cast<unsigned int>(luaL_checknumber(L, 1 + offset));
        g = static_cast<unsigned int>(luaL_checknumber(L, 2 + offset));
        b = static_cast<unsigned int>(luaL_checknumber(L, 3 + offset));
        color = {static_cast<float>(r) / 0xFF, static_cast<float>(g) / 0xFF, static_cast<float>(b) / 0xFF, 1.0F};
        lua_remove(L, 2 + offset);
        lua_remove(L, 2 + offset);
    }

    return color;
}

#include "LuaMatrix.h"
#include "LuaVector4.h"
#include "LuaVertex.h"
#include "LuaMaterial.h"
#include "LuaLight.h"
#include "LuaFaceGroup.h"
#include "LuaMesh.h"
#include "LuaNode.h"
#include "LuaScene.h"
#include "LuaRenderTarget.h"
#include "LuaEffect.h"
#include "LuaFont.h"
#include "LuaSound.h"
#include "LuaMusic.h"

/// BASE METHODS
LUAF(Base, ShowMessage)
{
    const auto* const caption = LuaGetInline<LPCSTR>(L);
    const auto* const text  = LuaGetInline<LPCSTR>(L);
    MessageBoxA(nullptr, text , caption, MB_OK);
    return 0;
}

LUAF(Base, LogString)
{
    const auto* const msg = LuaGetInline<LPCSTR>(L);
    PushLog(CString::Format("%s\n", msg).Str());
    return 0;
}

LUAF(Base, ExitGame)
{
    ENGINE->Shutdown();
    return 0;
}

LUAF(Base, RestartGame)
{
    VM->Restart();
    return 0;
}

LUAF(Base, IsDebugMode)
{
    #ifdef _DEBUG
    lua_pushboolean(L, TRUE);
    #else
	lua_pushboolean(L, FALSE);
    #endif
    return 1;
}

LUAF(Base, SetFPS)
{
    const auto fps = LuaGetInline<float>(L);
    ENGINE->SetFPS(fps);

    return 0;
}

LUAF(Base, dofile)
{
    const auto* const scriptName = LuaGetInline<LPCSTR>(L);
    const auto fd = FILESYSTEM->GetResource((LPSTR)scriptName);

    if (fd.data == nullptr)
    {
        VM->PostError("No dofile game script found!");
        return 0;
    }

    const int res = luaL_dostring(L, static_cast<char*>(fd.data));
    VM->CheckVMErrors(res);
    FILESYSTEM->FreeResource(fd.data);

    return 0;
}

LUAF(Base, loadfile)
{
    const auto* const scriptName = LuaGetInline<LPCSTR>(L);
    const auto fd = FILESYSTEM->GetResource((LPSTR)scriptName);

    if (fd.data == nullptr)
    {
        VM->PostError("No loadfile content found!");
        return 0;
    }

    lua_pushlstring(L, static_cast<char*>(fd.data), fd.size);
    FILESYSTEM->FreeResource(fd.data);

    return 1;
}

LUAF(Base, SaveState)
{
    const auto* const data = LuaGetInline<LPCSTR>(L);
    auto len = strlen(data);

    const LPCSTR out = b64_encode((unsigned char*)data, len);
    len = strlen(out);

    FILESYSTEM->SaveResource(out, len);
    neon_free((LPVOID)out);
    return 0;
}

LUAF(Base, LoadState)
{
    const auto f = FILESYSTEM->GetResource(RESOURCE_UDATA);

    if (f.data == nullptr)
    {
        lua_pushnil(L);
        return 1;
    }

    const auto* const out = static_cast<LPCSTR>(neon_malloc(f.size + 1));
    ZeroMemory((LPVOID)out, f.size+1);

    if (b64_decode(static_cast<const char*>(f.data), (unsigned char*)out, f.size) == 0)
    {
        neon_free((LPVOID)out);
        lua_pushnil(L);
        return 1;
    }

    lua_pushstring(L, out);
    neon_free((LPVOID)out);
    return 1;
}

LUAF(Base, getTime)
{
    lua_pushnumber(L, VM->GetRunTime());
    return 1;
}

///<END

void CLuaBindings::BindBase(lua_State* L)
{
    REGF(Base, ShowMessage);
    REGF(Base, LogString);
    REGF(Base, ExitGame);
    REGF(Base, IsDebugMode);
    REGF(Base, RestartGame);
    REGF(Base, SetFPS);
    REGF(Base, dofile);
    REGF(Base, loadfile);
    REGF(Base, SaveState);
    REGF(Base, LoadState);
    REGF(Base, getTime);
    REGFN(Base, "GetTime", getTime);
}

void CLuaBindings::BindAudio(lua_State* L)
{
    LuaSound_register(L);
    LuaMusic_register(L);
}

/// MATH METHODS
LUAF(Math, Color)
{
    const auto r = LuaGetInline<uint32_t>(L);
    const auto g = LuaGetInline<uint32_t>(L);
    const auto b = LuaGetInline<uint32_t>(L);
    unsigned int a = 0xFF;

    if (lua_gettop(L) >= 1)
    {
        a = LuaGetInline<uint32_t>(L);
    }

    lua_pushnumber(L, D3DCOLOR_ARGB(a, r, g, b));
    return 1;
}

LUAF(Math, ColorLinear)
{
    const float r = LuaGetInline<float>(L) / static_cast<float>(0xFF);
    const float g = LuaGetInline<float>(L) / static_cast<float>(0xFF);
    const float b = LuaGetInline<float>(L) / static_cast<float>(0xFF);
    float a = 0xFF;

    if (lua_gettop(L) >= 1)
    {
        a = LuaGetInline<float>(L) / static_cast<float>(0xFF);
    }

    lua_pushnumber(L, r);
    lua_pushnumber(L, g);
    lua_pushnumber(L, b);
    lua_pushnumber(L, a);
    return 4;
}

LUAF(Math, WorldToScreen)
{
    auto pos3D = LuaGetInline<D3DXVECTOR3>(L);
    auto view = LuaGetInline<D3DXMATRIX>(L);
    auto proj = LuaGetInline<D3DXMATRIX>(L);
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);

    auto* pos2D = reinterpret_cast<D3DXVECTOR3*>(vector4_ctor(L));
    D3DVIEWPORT9 viewport;
    RENDERER->GetDevice()->GetViewport(&viewport);

    D3DXVec3Project(pos2D, &pos3D, &viewport, &proj, &view, &world);
    return 1;
}

LUAF(Math, ScreenToWorld)
{
    auto pos2D = LuaGetInline<D3DXVECTOR3>(L);
    auto view = LuaGetInline<D3DXMATRIX>(L);
    auto proj = LuaGetInline<D3DXMATRIX>(L);
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);

    auto* pos3D = reinterpret_cast<D3DXVECTOR3*>(vector4_ctor(L));
    D3DVIEWPORT9 viewport;
    RENDERER->GetDevice()->GetViewport(&viewport);

    D3DXVec3Unproject(&pos2D, pos3D, &viewport, &proj, &view, &world);
    return 1;
}

///<END

void CLuaBindings::BindMath(lua_State* L)
{
    LuaMatrix_register(L);
    LuaVector_register(L);

    REGF(Math, Color);
    REGF(Math, WorldToScreen);
    REGF(Math, ScreenToWorld)
    REGF(Math, ColorLinear);
}

/// RENDERER METHODS
LUAF(Rend, ClearScene)
{
    RENDERER->ClearBuffer(luaH_getcolor(L), CLEARFLAG_STANDARD);
    return 0;
}

LUAF(Rend, CameraPerspective)
{
    const auto fov = LuaGetInline<float>(L);
    float zNear = 0.1f;
    float zFar = 1000.0f;
    bool flipHandedness = FALSE;

    if (LuaLength(L) >= 1)
    {
        zNear = LuaGetInline<float>(L);
        zFar = LuaGetInline<float>(L);
    }

    if (LuaLength(L) >= 1)
    {
        flipHandedness = LuaGetInline<bool>(L);
    }

    D3DXMATRIX matProjection;
    const RECT res = RENDERER->GetSurfaceResolution();

    if (flipHandedness)
    {
        D3DXMatrixPerspectiveFovRH(&matProjection,
                                   D3DXToRadian(fov),
                                   static_cast<float>(res.right) / static_cast<float>(res.bottom),
                                   zNear,
                                   zFar);
    }
    else
    {
        D3DXMatrixPerspectiveFovLH(&matProjection,
                                   D3DXToRadian(fov),
                                   static_cast<float>(res.right) / static_cast<float>(res.bottom),
                                   zNear,
                                   zFar);
    }

    RENDERER->SetMatrix(MATRIXKIND_PROJECTION, matProjection);

    return 0;
}

LUAF(Rend, CameraOrthographic)
{
    const RECT res = RENDERER->GetSurfaceResolution();
    auto w = static_cast<float>(res.right);
    auto h = static_cast<float>(res.bottom);
    bool flipHandedness = FALSE;

    if (LuaLength(L) >= 2)
    {
        w = LuaGetInline<float>(L) * (static_cast<float>(res.right) / static_cast<float>(res.bottom)
        );
        h = LuaGetInline<float>(L);
    }

    float zNear = 0.01f;
    float zFar = 100.0f;

    if (LuaLength(L) >= 2)
    {
        zNear = LuaGetInline<float>(L);
        zFar = LuaGetInline<float>(L);
    }

    if (LuaLength(L) >= 1)
    {
        flipHandedness = LuaGetInline<bool>(L);
    }

    D3DXMATRIX matProjection;

    if (flipHandedness)
    {
        D3DXMatrixOrthoRH(&matProjection,
                          w,
                          h,
                          zNear,
                          zFar);
    }
    else
    {
        D3DXMatrixOrthoLH(&matProjection,
                          w,
                          h,
                          zNear,
                          zFar);
    }

    RENDERER->SetMatrix(MATRIXKIND_PROJECTION, matProjection);

    return 0;
}

LUAF(Rend, CameraOrthographicEx)
{
    const RECT res = RENDERER->GetSurfaceResolution();
    float l = 0.0f;
    float t = 0.0f;
    auto r = static_cast<float>(res.right);
    auto b = static_cast<float>(res.bottom);
    bool flipHandedness = FALSE;

    if (LuaLength(L) >= 4)
    {
        l = LuaGetInline<float>(L);
        r = LuaGetInline<float>(L);
        b = LuaGetInline<float>(L);
        t = LuaGetInline<float>(L);
    }

    float zNear = 0.01f;
    float zFar = 100.0f;

    if (LuaLength(L) >= 2)
    {
        zNear = LuaGetInline<float>(L);
        zFar = LuaGetInline<float>(L);
    }

    if (LuaLength(L) >= 1)
    {
        flipHandedness = LuaGetInline<bool>(L);
    }

    D3DXMATRIX matProjection;

    if (flipHandedness)
    {
        D3DXMatrixOrthoOffCenterRH(&matProjection,
                                   l, r, b, t,
                                   zNear,
                                   zFar);
    }
    else
    {
        D3DXMatrixOrthoOffCenterLH(&matProjection,
                                   l, r, b, t,
                                   zNear,
                                   zFar);
    }

    RENDERER->SetMatrix(MATRIXKIND_PROJECTION, matProjection);

    return 0;
}

LUAF(Rend, BindTexture)
{
    const auto stage = LuaGetInline<DWORD>(L);
    CMaterial* tex = nullptr;

    if (LuaTestClass(L, L_RENDERTARGET))
    {
        auto* const rtt = LuaGetInline<CRenderTarget*>(L);
        RENDERER->SetTexture(stage, rtt->GetTextureHandle());
    }
    else if (LuaTestClass(L, L_MATERIAL))
    {
        auto* const mat = LuaGetInline<CMaterial*>(L);
        mat->Bind(stage);
        RENDERER->MarkMaterialOverride(true);
    }
    else if (LuaLength(L) >= 1)
    {
        auto* const handle = LuaGetInline<LPDIRECT3DTEXTURE9>(L);
        RENDERER->SetTexture(stage, handle);
    }
    else
    {
        RENDERER->GetDefaultMaterial()->Bind(stage);
        RENDERER->MarkMaterialOverride(false);
    }

    return 0;
}

LUAF(Rend, GetResolution)
{
    const RECT res = RENDERER->GetResolution();
    lua_newtable(L);

    // x
    lua_pushinteger(L, 1);
    lua_pushnumber(L, res.right);
    lua_settable(L, -3);

    // y
    lua_pushinteger(L, 2);
    lua_pushnumber(L, res.bottom);
    lua_settable(L, -3);

    return 1;
}

LUAF(Rend, GetMatrix)
{
    const auto kind = LuaGetInline<DWORD>(L);

    matrix_new(L);
    auto* mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 1, L_MATRIX));
    *mat = RENDERER->GetDeviceMatrix(kind);

    return 1;
}

LUAF(Rend, IsFocused)
{
    lua_pushboolean(L, RENDERER->IsFocused());
    return 1;
}

LUAF(Rend, RenderState)
{
    const auto kind = LuaGetInline<DWORD>(L);
    const bool state = LuaGetInline<bool>(L);

    RENDERER->SetRenderState(kind, static_cast<DWORD>(state));
    return 0;
}

LUAF(Rend, ToggleDepthTest)
{
    const bool state = LuaGetInline<bool>(L);
    RENDERER->SetRenderState(D3DRS_ZENABLE, static_cast<DWORD>(state));
    return 0;
}

LUAF(Rend, ToggleWireframe)
{
    const bool state = LuaGetInline<bool>(L);

    RENDERER->SetRenderState(D3DRS_FILLMODE, state ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
    RENDERER->SetRenderState(D3DRS_CULLMODE, state ? D3DCULL_NONE : D3DCULL_CCW);
    RENDERER->SetRenderState(D3DRS_ZENABLE, state ? FALSE : TRUE);
    return 0;
}

LUAF(Rend, SetFog)
{
    const DWORD color = luaH_getcolor(L);
    const auto mode = LuaGetInline<DWORD>(L);
    const auto start = LuaGetInline<float>(L);
    float end = 0.0F;

    if (mode == D3DFOG_LINEAR)
    {
        end = LuaGetInline<float>(L);
    }

    RENDERER->SetFog(color, mode, start, end);
    return 0;
}

LUAF(Rend, ClearFog)
{
    RENDERER->ClearFog();
    return 0;
}

LUAF(Rend, SamplerState)
{
    const auto stage = LuaGetInline<DWORD>(L);
    const auto kind = LuaGetInline<DWORD>(L);
    const auto state = LuaGetInline<DWORD>(L);

    RENDERER->SetSamplerState(stage, kind, state);
    return 0;
}

LUAF(Rend, EnableLighting)
{
    const bool state = LuaGetInline<bool>(L);
    RENDERER->EnableLighting(state);
    return 0;
}

LUAF(Rend, AmbientColor)
{
    if (LuaLength(L) == 0)
    {
        D3DCOLOR col = 0x0;
        RENDERER->GetDevice()->GetRenderState(D3DRS_AMBIENT, &col);
        lua_pushinteger(L, col);
        return 1;
    }

    RENDERER->SetRenderState(D3DRS_AMBIENT, luaH_getcolor(L));
    return 0;
}

LUAF(Rend, ClearTarget)
{
    RENDERER->SetRenderTarget(nullptr);
    return 0;
}

LUAF(Rend, DrawBox)
{
    const auto mat = LuaGetInline<D3DXMATRIX>(L);
    const auto dims = LuaGetInline<D3DXVECTOR4>(L);
    const auto color = LuaGetInline<DWORD>(L);

    RENDERER->DrawBox(mat, dims, color);
    return 0;
}

LUAF(Rend, ReadyAlphaBlend)
{
    RENDERER->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    RENDERER->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    RENDERER->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    RENDERER->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    RENDERER->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x08);
    RENDERER->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    RENDERER->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
    RENDERER->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
    RENDERER->GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
    return 0;
}

LUAF(Rend, DrawQuad)
{
    const auto x1 = LuaGetInline<float>(L);
    const auto x2 = LuaGetInline<float>(L);
    const auto y1 = LuaGetInline<float>(L);
    const auto y2 = LuaGetInline<float>(L);
    const auto color = LuaGetInline<DWORD>(L);
    const bool flipY = LuaGetInline<bool>(L);

    RENDERER->DrawQuad(x1, x2, y1, y2, color, flipY);
    return 0;
}

LUAF(Rend, DrawQuadEx)
{
    const auto pos = LuaGetInline<D3DXVECTOR3>(L);
    const auto w = LuaGetInline<float>(L);
    const auto h = LuaGetInline<float>(L);
    const auto color = LuaGetInline<DWORD>(L);
    const bool usesDepth = LuaGetInline<bool>(L);
    const bool flipY = LuaGetInline<bool>(L);

    RENDERER->DrawQuadEx(pos.x, pos.y, pos.z, w, h, color, usesDepth, flipY);
    return 0;
}

LUAF(Rend, DrawQuad3D)
{
    const auto x1 = LuaGetInline<float>(L);
    const auto x2 = LuaGetInline<float>(L);
    const auto y1 = LuaGetInline<float>(L);
    const auto y2 = LuaGetInline<float>(L);
    const auto z1 = LuaGetInline<float>(L);
    const auto z2 = LuaGetInline<float>(L);
    const auto color = LuaGetInline<DWORD>(L);

    RENDERER->DrawQuad3D(x1, x2, y1, y2, z1, z2, color);
    return 0;
}

LUAF(Rend, DrawPolygon)
{
    const auto a = LuaGetInline<VERTEX>(L);
    const auto b = LuaGetInline<VERTEX>(L);
    const auto c = LuaGetInline<VERTEX>(L);

    RENDERER->DrawPolygon(a, b, c);
    return 0;
}

LUAF(Rend, CullMode)
{
    const auto mode = LuaGetInline<DWORD>(L);
    RENDERER->SetRenderState(D3DRS_CULLMODE, mode);
    return 0;
}

LUAF(Rend, FillScreen)
{
    DWORD color = 0x00FFFFFF;
    bool flipY = FALSE;

    if (LuaLength(L) >= 1)
    {
        color = LuaGetInline<DWORD>(L);
    }

    if (LuaLength(L) >= 1)
    {
        flipY = LuaGetInline<bool>(L);
    }

    const RECT res = RENDERER->GetResolution();
    RENDERER->DrawQuad(0, static_cast<float>(res.right), 0, static_cast<float>(res.bottom), color, flipY);
    return 0;
}

LUAF(Rend, RegisterFontFile)
{
    const auto* const path = LuaGetInline<LPCSTR>(L);
    lua_pushboolean(L, static_cast<int>(CFont::AddFontToDatabase(path)));
    return 1;
}

///<END

void CLuaBindings::BindRenderer(lua_State* L)
{
    REGF(Rend, ClearScene);
    REGF(Rend, CameraPerspective);
    REGF(Rend, CameraOrthographic);
    REGF(Rend, CameraOrthographicEx);
    REGF(Rend, GetMatrix);
    REGF(Rend, GetResolution);
    REGF(Rend, IsFocused);
    REGF(Rend, RenderState);
    REGF(Rend, SetFog);
    REGF(Rend, ClearFog);
    REGF(Rend, ReadyAlphaBlend)
    REGF(Rend, SamplerState);
    REGF(Rend, ToggleWireframe);
    REGF(Rend, ToggleDepthTest);
    REGF(Rend, EnableLighting);
    REGF(Rend, CullMode);
    REGF(Rend, AmbientColor);
    REGF(Rend, ClearTarget);
    REGF(Rend, DrawBox)
    REGF(Rend, DrawQuad);
    REGF(Rend, DrawQuadEx)
    REGF(Rend, DrawQuad3D);
    REGF(Rend, DrawPolygon);
    REGF(Rend, FillScreen);
    REGF(Rend, RegisterFontFile);

    REGF(Rend, BindTexture);

    LuaVertex_register(L);
    LuaMaterial_register(L);
    LuaLight_register(L);
    LuaMesh_register(L);
    LuaFaceGroup_register(L);
    LuaScene_register(L);
    LuaNode_register(L);
    LuaEffect_register(L);
    LuaRenderTarget_register(L);
    LuaFont_register(L);

    // enums
    {
        REGE(PRIMITIVEKIND_POINTLIST);
        REGE(PRIMITIVEKIND_LINELIST);
        REGE(PRIMITIVEKIND_LINESTRIP);
        REGE(PRIMITIVEKIND_TRIANGLELIST);
        REGE(PRIMITIVEKIND_TRIANGLESTRIP);
        REGE(PRIMITIVEKIND_TRIANGLEFAN);

        REGE(MATRIXKIND_VIEW);
        REGE(MATRIXKIND_PROJECTION);
        REGE(MATRIXKIND_TEXTURE0);
        REGE(MATRIXKIND_TEXTURE1);
        REGE(MATRIXKIND_TEXTURE2);
        REGE(MATRIXKIND_TEXTURE3);
        REGE(MATRIXKIND_TEXTURE4);
        REGE(MATRIXKIND_TEXTURE5);
        REGE(MATRIXKIND_TEXTURE6);
        REGE(MATRIXKIND_TEXTURE7);
        REGE(MATRIXKIND_WORLD);

        REGE(CLEARFLAG_COLOR);
        REGE(CLEARFLAG_DEPTH);
        REGE(CLEARFLAG_STENCIL);
        REGE(CLEARFLAG_STANDARD);

        REGN(TEXTURESLOT_ALBEDO, TEXTURESLOT_ALBEDO + 1);
        REGN(TEXTURESLOT_SPECULAR, TEXTURESLOT_SPECULAR + 1);
        REGN(TEXTURESLOT_NORMAL, TEXTURESLOT_NORMAL + 1);
        REGN(TEXTURESLOT_DISPLACE, TEXTURESLOT_DISPLACE + 1);
        REGN(TEXTURESLOT_USER_END, TEXTURESLOT_USER_END + 1);
        REGN(MAX_TEXTURE_SLOTS, MAX_TEXTURE_SLOTS + 1);

        REGN(FOGKIND_NONE, 0);
        REGN(FOGKIND_EXP, 1);
        REGN(FOGKIND_EXP2, 2);
        REGN(FOGKIND_LINEAR, 3);

        // helpers
        REGN(WORLD, MATRIXKIND_WORLD);
        REGN(VIEW, MATRIXKIND_VIEW);
        REGN(PROJ, MATRIXKIND_PROJECTION);

        REGN(CULLKIND_NONE, 1);
        REGN(CULLKIND_CW, 2);
        REGN(CULLKIND_CCW, 3);

        // render states
        #define _X(NAME, VALUE) REGN(NAME, VALUE);
        #include "RenderStates.h"
        #undef _X

        // light types
        REGN(LIGHTKIND_DIRECTIONAL, D3DLIGHT_DIRECTIONAL);
        REGN(LIGHTKIND_POINT, D3DLIGHT_POINT);
        REGN(LIGHTKIND_SPOT, D3DLIGHT_SPOT);
    }
}

/// INPUT METHODS
static auto GetScanCodeFromLua(lua_State* L) -> DWORD
{
    if (lua_isinteger(L, 1) != 0)
    {
        return static_cast<DWORD>(lua_tointeger(L, 1));
    }

    if (lua_isstring(L, 1) != 0)
    {
        const CHAR* str = lua_tostring(L, 1);

        if (strlen(str) != 1)
        {
            return 0x0;
        }

        const CHAR c = tolower(str[0]);

        if (isalpha(c) != 0)
        {
            const CHAR dist = c - 'a';
            return 0x41 + dist;
        }

        if (isdigit(c) != 0)
        {
            const CHAR dist = c - '0';
            return 0x30 + dist;
        }
    }

    return 0x0;
}

LUAF(Input, IsCursorVisible)
{
    lua_pushboolean(L, INPUT->GetCursor());
    return 1;
}

LUAF(Input, ShowCursor)
{
    const bool state = LuaGetInline<bool>(L);
    INPUT->SetCursor(state);
    return 0;
}

LUAF(Input, GetKey)
{
    const DWORD code = GetScanCodeFromLua(L);
    lua_pushboolean(L, INPUT->GetKey(code));
    return 1;
}

LUAF(Input, GetKeyDown)
{
    const DWORD code = GetScanCodeFromLua(L);
    lua_pushboolean(L, INPUT->GetKeyDown(code));
    return 1;
}

LUAF(Input, GetKeyUp)
{
    const DWORD code = GetScanCodeFromLua(L);
    lua_pushboolean(L, INPUT->GetKeyUp(code));
    return 1;
}

LUAF(Input, GetMouse)
{
    const auto code = LuaGetInline<DWORD>(L);
    lua_pushboolean(L, INPUT->GetMouse(code));
    return 1;
}

LUAF(Input, GetMouseDown)
{
    const auto code = LuaGetInline<DWORD>(L);
    lua_pushboolean(L, INPUT->GetMouseDown(code));
    return 1;
}

LUAF(Input, GetMouseUp)
{
    const auto code = LuaGetInline<DWORD>(L);
    lua_pushboolean(L, INPUT->GetMouseUp(code));
    return 1;
}

LUAF(Input, GetMouseXY)
{
    const POINT pos = INPUT->GetMouseXY();
    lua_newtable(L);

    // x
    lua_pushinteger(L, 1);
    lua_pushnumber(L, pos.x);
    lua_settable(L, -3);

    // y
    lua_pushinteger(L, 2);
    lua_pushnumber(L, pos.y);
    lua_settable(L, -3);

    return 1;
}

LUAF(Input, GetMouseDelta)
{
    const POINT pos = INPUT->GetMouseDelta();
    lua_newtable(L);

    // x
    lua_pushinteger(L, 1);
    lua_pushnumber(L, pos.x);
    lua_settable(L, -3);

    // y
    lua_pushinteger(L, 2);
    lua_pushnumber(L, pos.y);
    lua_settable(L, -3);

    return 1;
}

LUAF(Input, SetMouseXY)
{
    const auto x = LuaGetInline<short>(L);
    const auto y = LuaGetInline<short>(L);
    INPUT->SetMouseXY(x, y);

    return 0;
}

LUAF(Input, GetCursorMode)
{
    lua_pushinteger(L, INPUT->GetCursorMode());
    return 1;
}

LUAF(Input, SetCursorMode)
{
    const auto mode = LuaGetInline<int>(L);
    INPUT->SetCursorMode(static_cast<UCHAR>(mode));
    return 0;
}

///<END

void CLuaBindings::BindInput(lua_State* L)
{
    REGF(Input, GetKey);
    REGF(Input, GetKeyDown);
    REGF(Input, GetKeyUp);
    REGF(Input, GetMouseXY);
    REGF(Input, GetMouseDelta);
    REGF(Input, SetMouseXY);
    REGF(Input, GetMouse);
    REGF(Input, GetMouseDown);
    REGF(Input, GetMouseUp);

    REGF(Input, IsCursorVisible);
    REGF(Input, ShowCursor);

    REGF(Input, GetCursorMode);
    REGF(Input, SetCursorMode);
    REGF(Input, ShowCursor);

    // keys
    {
        #define _X(NAME, VALUE) REGN(NAME, VALUE)
        #include "InputCodes.h"
        #undef _X
    }

    // mouse buttons
    {
        REGN(MOUSE_LEFT_BUTTON, CInput::MOUSE_LEFT_BUTTON);
        REGN(MOUSE_MIDDLE_BUTTON, CInput::MOUSE_MIDDLE_BUTTON);
        REGN(MOUSE_RIGHT_BUTTON, CInput::MOUSE_RIGHT_BUTTON);
        REGN(MOUSE_WHEEL_UP, CInput::MOUSE_WHEEL_UP);
        REGN(MOUSE_WHEEL_DOWN, CInput::MOUSE_WHEEL_DOWN);
    }

    // cursor modes
    {
        REGE(CURSORMODE_DEFAULT);
        REGE(CURSORMODE_CENTERED);
        REGE(CURSORMODE_WRAPPED);
    }
}
