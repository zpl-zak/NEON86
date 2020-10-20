#include "stdafx.h"
#include "Renderer.h"

#include <cmath>
#include "Material.h"
#include "FaceGroup.h"
#include "Effect.h"
#include "RenderTarget.h"
#include "ProfileManager.h"

#include "engine.h"
#include "UserInterface.h"
#include "VM.h"

#if !defined(_DEBUG) || defined(NEON_FORCE_D3DX9)
#pragma comment (lib, "d3d9/d3dx9.lib")
#else
#pragma comment (lib, "d3d9/d3dx9d.lib")
#endif

#pragma comment (lib, "d3d9/dxguid.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderer::CRenderer()
{
    mDirect9 = nullptr;
    mDevice = nullptr;
    mWindow = nullptr;
    mMainTarget = nullptr;
    mActiveEffect = nullptr;
    mDefaultBox = nullptr;
    mDefaultMaterial = nullptr;
    mVsync = TRUE;
    mFullscreen = FALSE;
    mEnableLighting = FALSE;
    mUsesMaterialOverride = FALSE;
    ZeroMemory(&mLastRes, sizeof(mLastRes));
    ZeroMemory(&mParams, sizeof(mParams));
}

void CRenderer::BuildParams()
{
    ZeroMemory(&mParams, sizeof(mParams));
    mParams.Windowed = TRUE;
    mParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    mParams.hDeviceWindow = mWindow;
    mParams.EnableAutoDepthStencil = TRUE;
    mParams.AutoDepthStencilFormat = D3DFMT_D24S8;

    if (mVsync)
    {
        mParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        mParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    }
    else
    {
        mParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        mParams.FullScreen_RefreshRateInHz = 0;
    }
}

void CRenderer::PrepareEffectDraw()
{
    if (GetActiveEffect() != nullptr)
    {
        CEffect* activeEffect = GetActiveEffect();
        D3DXMATRIX world = GetDeviceMatrix(MATRIXKIND_WORLD);
        D3DXMATRIX view = GetDeviceMatrix(MATRIXKIND_VIEW);
        D3DXMATRIX proj = GetDeviceMatrix(MATRIXKIND_PROJECTION);
        D3DXMATRIX inverseWorld;
        D3DXMATRIX inverseMV;
        D3DXMatrixInverse(&inverseWorld, nullptr, &world);

        D3DXMATRIX mvp = world * view * proj;
        D3DXMATRIX mv = world * view;

        D3DXMatrixInverse(&inverseMV, nullptr, &mv);

        activeEffect->SetMatrix("NEON.World", world);
        activeEffect->SetMatrix("NEON.InverseWorld", inverseWorld, TRUE);
        activeEffect->SetMatrix("NEON.InverseWorldView", inverseMV, TRUE);
        activeEffect->SetMatrix("NEON.WorldView", mv);
        activeEffect->SetMatrix("NEON.MVP", mvp);

        D3DCOLOR ambience = 0x0;
        GetDevice()->GetRenderState(D3DRS_AMBIENT, &ambience);
        activeEffect->SetInteger("NEON.AmbientColor", ambience);

        activeEffect->CommitChanges();
    }
}

auto CRenderer::CreateDevice(HWND window, RECT winres) -> LRESULT
{
    mWindow = window;
    mDirect9 = Direct3DCreate9(D3D_SDK_VERSION);

    if (mDirect9 == nullptr)
    {
        return ERROR_INVALID_HANDLE;
    }

    BuildParams();

    D3DDISPLAYMODE mode;
    mDirect9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

    LRESULT res = mDirect9->CheckDeviceType(D3DADAPTER_DEFAULT,
                                            D3DDEVTYPE_HAL,
                                            mode.Format,
                                            D3DFMT_A8R8G8B8,
                                            1);

    if (FAILED(res))
    {
        return res;
    }

    D3DCAPS9 caps;

    res = mDirect9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

    int capflags = D3DCREATE_FPU_PRESERVE;

    if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0u)
    {
        capflags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }
    else
    {
        capflags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    if ((caps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0u)
    {
        capflags |= D3DCREATE_PUREDEVICE;
    }

    res = mDirect9->CreateDevice(D3DADAPTER_DEFAULT,
                                 D3DDEVTYPE_HAL,
                                 window,
                                 capflags,
                                 &mParams,
                                 &mDevice);

    if (mDevice == nullptr)
    {
        return res;
    }

    LPDIRECT3DSURFACE9 display;
    mDevice->GetRenderTarget(0, &display);
    display->GetDesc(&mDisplayDesc);
    display->Release();

    winres.right = mDisplayDesc.Width;
    winres.bottom = mDisplayDesc.Height;

    Resize(winres);

    mMainTarget = new CRenderTarget(winres.right, winres.bottom, FALSE);
    SetRenderTarget(nullptr);
    mDefaultMaterial = new CMaterial();

    return res;
}

void CRenderer::ResetDevice()
{
    if (mDevice == nullptr)
    {
        return;
    }

    BuildParams();
    mDevice->Reset(&mParams);

    SetDefaultRenderStates();
}

void CRenderer::SetVSYNC(bool state)
{
    mVsync = state;
    ResetDevice();
}

void CRenderer::Blit()
{
    DrawQuad(0, 0, 0, 0, D3DCOLOR_XRGB(255, 255, 255));
    IDirect3DSurface9* bbuf = nullptr;
    mDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bbuf);
    mDevice->StretchRect(
        mMainTarget->GetSurfaceHandle(),
        nullptr,
        bbuf,
        nullptr,
        D3DTEXF_NONE);
    bbuf->Release();
}

void CRenderer::BeginRender() const
{
    if (mDevice->TestCooperativeLevel() == D3DERR_DEVICELOST)
    {
        return;
    }

    if (mDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
    {
        MessageBoxA(mWindow, "Device has been lost. (Don't leave the game on next time :( )", "Direct3D Error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    SetDefaultRenderStates();
    GetDevice()->BeginScene();
}

void CRenderer::EndRender()
{
    Blit();
    GetDevice()->EndScene();
    GetDevice()->Present(nullptr, nullptr, nullptr, nullptr);
    ENGINE->DefaultProfiling.IncrementFrame();
}

auto CRenderer::Release() -> bool
{
    SAFE_RELEASE(mMainTarget);
    SAFE_RELEASE(mDevice);
    SAFE_RELEASE(mDirect9);
    SAFE_RELEASE(mDefaultMaterial);

    return TRUE;
}

void CRenderer::Resize(RECT res)
{
    if (mDevice == nullptr)
    {
        return;
    }

    D3DVIEWPORT9 vp;
    vp.X = 0;
    vp.Y = 0;
    vp.Width = res.right;
    vp.Height = res.bottom;
    vp.MinZ = 0.0F;
    vp.MaxZ = 1.0F;

    mDevice->SetViewport(&vp);

    #if 0
    MessageBoxA(NULL, "Window resizing not supported yet!", "Renderer error", MB_OK);
    TerminateProcess(NULL, 1);
    return;
    #endif

    mLastRes = res;
}

/// Render commands
void CRenderer::DrawMesh(const RENDERDATA& data)
{
    D3DMATRIX oldMatrix;
    if (data.usesMatrix)
    {
        oldMatrix = GetDeviceMatrix(MATRIXKIND_WORLD);
        SetMatrix(MATRIXKIND_WORLD, data.matrix);
    }

    PrepareEffectDraw();

    if (data.mesh != nullptr)
    {
        data.mesh->DrawSubset(0);
    }

    if (data.usesMatrix)
    {
        SetMatrix(MATRIXKIND_WORLD, oldMatrix);
    }
}

void CRenderer::DrawPolygon(const VERTEX& a, const VERTEX& b, const VERTEX& c)
{
    D3DXVECTOR3 v1 = D3DXVECTOR3(a.x, a.y, a.z);
    D3DXVECTOR3 v2 = D3DXVECTOR3(b.x, b.y, b.z);
    D3DXVECTOR3 v3 = D3DXVECTOR3(c.x, c.y, c.z);
    D3DXVECTOR3 e1 = (v3 - v1);
    D3DXVECTOR3 e2 = (v3 - v2);
    D3DXVECTOR3 n;
    D3DXVec3Cross(&n, &e1, &e2);

    mDevice->SetRenderState(D3DRS_LIGHTING, static_cast<DWORD>(IsLightingEnabled()));

    ZeroMemory(&mImmediateBuffer, sizeof(mImmediateBuffer));
    mImmediateBuffer[0] = {a.x, a.y, a.z, n.x, n.y, n.z, 0, 0, 0, 0, 0, 0, a.color, a.su, a.tv};
    mImmediateBuffer[1] = {b.x, b.y, b.z, n.x, n.y, n.z, 0, 0, 0, 0, 0, 0, b.color, b.su, b.tv};
    mImmediateBuffer[2] = {c.x, c.y, c.z, n.x, n.y, n.z, 0, 0, 0, 0, 0, 0, c.color, c.su, c.tv};

    static IDirect3DVertexDeclaration9* vertsDecl = nullptr;

    if (vertsDecl == nullptr)
    {
        mDevice->CreateVertexDeclaration(meshVertexFormat, &vertsDecl);
    }

    mDevice->SetVertexDeclaration(vertsDecl);
    mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, static_cast<LPVOID>(mImmediateBuffer), sizeof(VERTEX));
}

void CRenderer::DrawQuad3D(float x1, float x2, float y1, float y2, float z1, float z2, DWORD color)
{
    VERTEX verts[] =
    {
        {x1, y2, z2, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, 0.0F, 0.0F},
        {x2, y1, z1, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, 1.0F, 1.0F},
        {x2, y2, z2, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, 1.0F, 0.0F},

        {x1, y2, z2, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, 0.0F, 0.0F},
        {x1, y1, z1, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, 0.0F, 1.0F},
        {x2, y1, z1, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, 1.0F, 1.0F},
    };

    mDevice->SetRenderState(D3DRS_LIGHTING, static_cast<DWORD>(IsLightingEnabled()));

    static IDirect3DVertexDeclaration9* vertsDecl = nullptr;

    if (vertsDecl == nullptr)
    {
        mDevice->CreateVertexDeclaration(meshVertexFormat, &vertsDecl);
    }

    mDevice->SetVertexDeclaration(vertsDecl);
    mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, static_cast<LPVOID>(verts), sizeof(VERTEX));
}

void CRenderer::DrawQuad(float x1, float x2, float y1, float y2, DWORD color, bool flipY)
{
    VERTEX_2D verts[] =
    {
        {x1, y2, 0, 1, color, 0.0F, static_cast<float>(flipY) - 0.0F},
        {x2, y1, 0, 1, color, 1.0F, static_cast<float>(flipY) - 1.0F},
        {x2, y2, 0, 1, color, 1.0F, static_cast<float>(flipY) - 0.0F},

        {x1, y2, 0, 1, color, 0.0F, static_cast<float>(flipY) - 0.0F},
        {x1, y1, 0, 1, color, 0.0F, static_cast<float>(flipY) - 1.0F},
        {x2, y1, 0, 1, color, 1.0F, static_cast<float>(flipY) - 1.0F},
    };

    mDevice->SetRenderState(D3DRS_LIGHTING, static_cast<DWORD>(IsLightingEnabled()));

    static IDirect3DVertexDeclaration9* vertsDecl = nullptr;

    if (vertsDecl == nullptr)
    {
        mDevice->CreateVertexDeclaration(meshVertex2DFormat, &vertsDecl);
    }

    mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    mDevice->SetVertexDeclaration(vertsDecl);
    mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, static_cast<void*>(verts), sizeof(VERTEX_2D));
    mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

void CRenderer::DrawQuadEx(float x, float y, float z, float w, float h, DWORD color, bool usesDepth, bool flipY)
{
    VERTEX_2D verts[] =
    {
        {x, (y + h), z, 1, color, 0.0F, static_cast<float>(flipY) - 0.0F},
        {(x + w), y, z, 1, color, 1.0F, static_cast<float>(flipY) - 1.0F},
        {(x + w), (y + h), z, 1, color, 1.0F, static_cast<float>(flipY) - 0.0F},

        {x, (y + h), z, 1, color, 0.0F, static_cast<float>(flipY) - 0.0F},
        {x, y, z, 1, color, 0.0F, static_cast<float>(flipY) - 1.0F},
        {(x + w), y, z, 1, color, 1.0F, static_cast<float>(flipY) - 1.0F},
    };

    mDevice->SetRenderState(D3DRS_LIGHTING, static_cast<DWORD>(IsLightingEnabled()));

    static IDirect3DVertexDeclaration9* vertsDecl = nullptr;

    if (vertsDecl == nullptr)
    {
        mDevice->CreateVertexDeclaration(meshVertex2DFormat, &vertsDecl);
    }

    mDevice->SetRenderState(D3DRS_ZENABLE, static_cast<DWORD>(usesDepth));
    mDevice->SetVertexDeclaration(vertsDecl);
    mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, static_cast<void*>(verts), sizeof(VERTEX_2D));
    mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CRenderer::DrawBox(D3DXMATRIX mat, D3DXVECTOR4 dims, DWORD color)
{
    SAFE_RELEASE(mDefaultBox);

    SetMatrix(MATRIXKIND_WORLD, mat);

    D3DXCreateBox(mDevice, static_cast<float>(std::fabs(static_cast<float>(dims.x))),
                  static_cast<float>(std::fabs(static_cast<float>(dims.y))),
                  static_cast<float>(std::fabs(static_cast<float>(dims.z))), &mDefaultBox, nullptr);

    if (mDefaultBox == nullptr)
    {
        return;
    }

    mDevice->SetRenderState(D3DRS_LIGHTING, static_cast<DWORD>(IsLightingEnabled()));

    mDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CONSTANT);
    mDevice->SetTextureStageState(0, D3DTSS_CONSTANT, color);
    mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    mDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    mDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CONSTANT);
    mDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    mDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    mDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    PrepareEffectDraw();
    mDefaultBox->DrawSubset(0);
}

void CRenderer::ClearBuffer(D3DCOLOR color, unsigned int flags)
{
    mDevice->Clear(0, nullptr, flags, color, 1.0F, 0);
}

void CRenderer::SetMaterial(DWORD stage, CMaterial* mat)
{
    if ((GetActiveEffect() != nullptr) && (mat != nullptr))
    {
        const auto* const fx = GetActiveEffect();
        const auto matData = mat->GetMaterialData();

        fx->SetColor("MAT.Diffuse", matData.Diffuse);
        fx->SetColor("MAT.Ambient", matData.Ambient);
        fx->SetColor("MAT.Specular", matData.Specular);
        fx->SetColor("MAT.Emissive", matData.Emissive);
        fx->SetFloat("MAT.Power", matData.Power);
        fx->SetFloat("MAT.Opacity", matData.Opacity);
        fx->SetBool("MAT.IsShaded", matData.Shaded);
        fx->SetBool("MAT.AlphaIsTransparency", matData.AlphaIsTransparency);

        fx->SetTexture("diffuseTex", mat->GetTextureHandle(TEXTURESLOT_ALBEDO));
        fx->SetBool("hasDiffuseTex", mat->GetTextureHandle(TEXTURESLOT_ALBEDO) != nullptr);

        fx->SetTexture("specularTex", mat->GetTextureHandle(TEXTURESLOT_SPECULAR));
        fx->SetBool("hasSpecularTex", mat->GetTextureHandle(TEXTURESLOT_SPECULAR) != nullptr);

        fx->SetTexture("normalTex", mat->GetTextureHandle(TEXTURESLOT_NORMAL));
        fx->SetBool("hasNormalTex", mat->GetTextureHandle(TEXTURESLOT_NORMAL) != nullptr);

        fx->SetTexture("dispTex", mat->GetTextureHandle(TEXTURESLOT_DISPLACE));
        fx->SetBool("hasDispTex", mat->GetTextureHandle(TEXTURESLOT_DISPLACE) != nullptr);
        fx->CommitChanges();
    }
    else if (GetActiveEffect() != nullptr)
    {
        CEffect* fx = GetActiveEffect();

        fx->SetTexture("diffuseTex", nullptr);
        fx->SetBool("hasDiffuseTex", false);

        fx->SetTexture("specularTex", nullptr);
        fx->SetBool("hasSpecularTex", false);

        fx->SetTexture("normalTex", nullptr);
        fx->SetBool("hasNormalTex", false);

        fx->SetTexture("dispTex", nullptr);
        fx->SetBool("hasDispTex", false);
    }
    else
    {
        MATERIAL* matData = mat != nullptr ? &mat->GetMaterialData() : nullptr;
        SetTexture(stage, mat != nullptr ? mat->GetTextureHandle() : nullptr);
        if (mat != nullptr)
        {
            mDevice->SetMaterial(matData);
        }
        else
        {
            mDevice->SetMaterial(&mDefaultMaterial->GetMaterialData());
        }

        if ((mat != nullptr) && mat->IsTransparent())
        {
            mDevice->SetTextureStageState(stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            mDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            mDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG2, D3DTA_CONSTANT);

            DWORD alphaColor = D3DCOLOR_ARGB(matData ? static_cast<DWORD>(matData->Opacity * 255.0F) : 255, 255, 255,
                                             255);
            mDevice->SetTextureStageState(stage, D3DTSS_CONSTANT, alphaColor);

            mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

            if ((matData != nullptr) && static_cast<int>(matData->AlphaIsTransparency) == TRUE && static_cast<int>(
                matData->AlphaTestEnabled) == TRUE)
            {
                mDevice->SetRenderState(D3DRS_ALPHAREF, static_cast<DWORD>(matData->AlphaRef));
                mDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
                mDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
            }

            mDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            mDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            mDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        }
        else
        {
            mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            mDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        }
    }
}

void CRenderer::SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 handle)
{
    mDevice->SetTextureStageState(stage, D3DTSS_COLOROP, handle != nullptr ? D3DTOP_MODULATE : D3DTOP_SELECTARG2);
    mDevice->SetTexture(stage, handle);
}

void CRenderer::SetMatrix(unsigned int kind, const D3DXMATRIX& mat)
{
    mDevice->SetTransform(static_cast<D3DTRANSFORMSTATETYPE>(kind), &mat);
}

void CRenderer::ResetMatrices()
{
    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);

    mDevice->SetTransform(D3DTS_WORLD, &mat);
    mDevice->SetTransform(D3DTS_VIEW, &mat);
    mDevice->SetTransform(D3DTS_PROJECTION, &mat);
}

void CRenderer::SetRenderTarget(CRenderTarget* target)
{
    if ((target != nullptr) && (target->GetSurfaceHandle() != nullptr))
    {
        mDevice->SetRenderTarget(0, target->GetSurfaceHandle());
        mDevice->SetDepthStencilSurface(target->GetKind() == RTKIND_DEPTH
                                            ? nullptr
                                            : target->GetDepthStencilSurfaceHandle());
        mActiveTarget = target;
    }
    else
    {
        mDevice->SetRenderTarget(0, mMainTarget->GetSurfaceHandle());
        mDevice->SetDepthStencilSurface(mMainTarget->GetDepthStencilSurfaceHandle());
        mActiveTarget = mMainTarget;
    }
}

void CRenderer::SetRenderState(DWORD kind, DWORD value)
{
    mDevice->SetRenderState(static_cast<D3DRENDERSTATETYPE>(kind), static_cast<DWORD>(value));
}

void CRenderer::SetSamplerState(DWORD stage, DWORD kind, DWORD value)
{
    mDevice->SetSamplerState(stage, static_cast<D3DSAMPLERSTATETYPE>(kind), value);
}

void CRenderer::SetFog(DWORD color, DWORD mode, float start, float end)
{
    mDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    mDevice->SetRenderState(D3DRS_FOGTABLEMODE, mode);

    if (mode == D3DFOG_LINEAR)
    {
        mDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&start));
        mDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&end));
    }
    else
    {
        mDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)(&start));
    }

    mDevice->SetRenderState(D3DRS_FOGCOLOR, color);
}

void CRenderer::ClearFog()
{
    mDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

auto CRenderer::GetSurfaceResolution() -> RECT
{
    D3DSURFACE_DESC desc;
    mActiveTarget->GetTextureHandle()->GetLevelDesc(0, &desc);

    RECT r;
    r.left = r.top = 0;
    r.right = desc.Width;
    r.bottom = desc.Height;
    return r;
}

void CRenderer::SetDefaultRenderStates() const
{
    mDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
    mDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    mDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
    mDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

    mDevice->SetRenderState(D3DRS_LIGHTING, static_cast<DWORD>(IsLightingEnabled()));
    mDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    mDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
                            D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE |
                            D3DCOLORWRITEENABLE_ALPHA);
    mDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    mDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    mDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
}

auto CRenderer::GetDeviceMatrix(unsigned int kind) const -> D3DMATRIX
{
    D3DMATRIX mat;

    mDevice->GetTransform(static_cast<D3DTRANSFORMSTATETYPE>(kind), &mat);
    return mat;
}

auto CRenderer::GetLocalCoordinates() const -> RECT
{
    RECT Rect;
    GetWindowRect(mWindow, &Rect);
    MapWindowPoints(HWND_DESKTOP, GetParent(mWindow), (LPPOINT)&Rect, 2);
    return Rect;
}

D3DVERTEXELEMENT9 meshVertexFormat[] =
{
    {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
    {0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
    {0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
    {0, 48, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    {0, 52, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    {0, 60, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
    D3DDECL_END()
};

D3DVERTEXELEMENT9 meshVertex2DFormat[] =
{
    {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0},
    {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    {0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    D3DDECL_END()
};

/// Following lines make sure game runs on your dedicated mobile GPU rather than integrated one. (Applies only to mobile devices with integrated and dedicated GPU)
_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; // NVIDIA
_declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1; // ATI/AMD
