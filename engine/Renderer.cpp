// Renderer.cpp: implementation of the CRenderer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Renderer.h"
#include "Material.h"
#include "Frustum.h"
#include "FaceGroup.h"
#include "Effect.h"
#include "RenderTarget.h"

#include "StdAfx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderer::CRenderer()
{
	mFrustum = new CFrustum();
	mDirect9 = NULL;
	mDevice = NULL;
	mWindow = NULL;
	mMainTarget = NULL;
	mActiveEffect = NULL;
	mVsync = TRUE;
	mFullscreen = FALSE;
	ZeroMemory(&mLastRes, sizeof(mLastRes));
	ZeroMemory(&mParams, sizeof(mParams));
}

VOID CRenderer::BuildParams(VOID)
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

LRESULT CRenderer::CreateDevice(HWND window, RECT winres)
{
	mWindow = window;
	mDirect9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (mDirect9 == NULL)
		return ERROR_INVALID_HANDLE;

	BuildParams();

	D3DDISPLAYMODE mode;
	mDirect9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

	LRESULT res = mDirect9->CheckDeviceType(D3DADAPTER_DEFAULT,
											D3DDEVTYPE_HAL,
											mode.Format,
											D3DFMT_A8R8G8B8,
											true);

	if (FAILED(res))
		return res;

	D3DCAPS9 caps;

	res = mDirect9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	INT capflags = D3DCREATE_FPU_PRESERVE;

	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		capflags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		capflags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

    if (caps.DevCaps & D3DDEVCAPS_PUREDEVICE) {
        capflags |= D3DCREATE_PUREDEVICE;
    }

	res = mDirect9->CreateDevice(D3DADAPTER_DEFAULT,
							D3DDEVTYPE_HAL,
							window,
							capflags,
							&mParams,
							&mDevice);

	if (!mDevice)
		return res;

	Resize(winres);

	LPDIRECT3DSURFACE9 display;
	mDevice->GetRenderTarget(0, &display);
	display->GetDesc(&mDisplayDesc);
	display->Release();

	mMainTarget = new CRenderTarget(mLastRes.right, mLastRes.bottom, TRUE);
	SetRenderTarget(NULL);

	return res;
}

VOID CRenderer::ResetDevice(VOID)
{
	if (!mDevice)
		return;

	BuildParams();
	mDevice->Reset(&mParams);

	SetDefaultRenderStates();
}

VOID CRenderer::SetVSYNC(BOOL state)
{
	mVsync = state;
	ResetDevice();
}

VOID CRenderer::Blit(VOID)
{
	DrawQuad(0, 0, 0, 0, D3DCOLOR_XRGB(255, 255, 255));
    IDirect3DSurface9* bbuf = NULL;
    mDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bbuf);
	mDevice->StretchRect(
		mMainTarget->GetSurfaceHandle(),
		NULL,
		bbuf,
		NULL,
		D3DTEXF_NONE);
	bbuf->Release();
}

VOID CRenderer::Clear(VOID)
{

}

BOOL CRenderer::Release(VOID)
{
	SAFE_RELEASE(mMainTarget);
	SAFE_RELEASE(mFrustum);
	SAFE_RELEASE(mDevice);
	SAFE_RELEASE(mDirect9);

	return TRUE;
}

VOID CRenderer::Resize(RECT res)
{
	if (!mDevice)
		return;

	D3DVIEWPORT9 vp;
	vp.X      = 0;
	vp.Y      = 0;
	vp.Width  = res.right;
	vp.Height = res.bottom;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;

	mDevice->SetViewport(&vp);

#if 0
    MessageBoxA(NULL, "Window resizing not supported yet!", "Renderer error", MB_OK);
	TerminateProcess(NULL, 1);
    return;
#endif

	mLastRes = res;
}

/// Render commands
VOID CRenderer::DrawMesh(const RENDERDATA& data)
{
	if (data.usesMatrix)
		SetMatrix(MATRIXKIND_WORLD, data.matrix);

	if (GetActiveEffect())
	{
		D3DXMATRIX world = GetDeviceMatrix(MATRIXKIND_WORLD);
		D3DXMATRIX view  = GetDeviceMatrix(MATRIXKIND_VIEW);
		D3DXMATRIX proj  = GetDeviceMatrix(MATRIXKIND_PROJECTION);
		D3DXMATRIX inverseWorld, inverseMV;
		D3DXMatrixInverse(&inverseWorld, NULL, &world);

        D3DXMATRIX mvp = world * view * proj;
		D3DXMATRIX mv = world * view;

		D3DXMatrixInverse(&inverseMV, NULL, &mv);

		GetActiveEffect()->SetMatrix("NEON.World", world);
		GetActiveEffect()->SetMatrix("NEON.InverseWorld", inverseWorld, TRUE);
		GetActiveEffect()->SetMatrix("NEON.InverseWorldView", inverseMV, TRUE);
		GetActiveEffect()->SetMatrix("NEON.WorldView", mv);
		GetActiveEffect()->SetMatrix("NEON.MVP", mvp);
        GetActiveEffect()->CommitChanges();
	}

	data.mesh->DrawSubset(0);
}

VOID CRenderer::DrawQuad(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, DWORD color, BOOL flipY)
{
	VERTEX_2D verts[] = 
	{
		{x1, y2, 0, 1, color, 0.0f, flipY - 0.0f},
		{x2, y1, 0, 1, color, 1.0f, flipY - 1.0f},
		{x2, y2, 0, 1, color, 1.0f, flipY - 0.0f},

		{x1, y2, 0, 1, color, 0.0f, flipY - 0.0f},
		{x1, y1, 0, 1, color, 0.0f, flipY - 1.0f},
		{x2, y1, 0, 1, color, 1.0f, flipY - 1.0f},
	};

	static IDirect3DVertexDeclaration9* vertsDecl = NULL;

	if (!vertsDecl)
		mDevice->CreateVertexDeclaration(meshVertex2DFormat, &vertsDecl);

	mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	mDevice->SetVertexDeclaration(vertsDecl);
	mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 6, (VOID*)verts, sizeof(VERTEX_2D));
	mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

VOID CRenderer::ClearBuffer(D3DCOLOR color, UINT flags)
{
	mDevice->Clear(0, NULL, flags, color, 1.0f, 1);
}

VOID CRenderer::SetMaterial(DWORD stage, CMaterial* mat)
{
    if (GetActiveEffect() && mat)
    {
        CEffect* fx = GetActiveEffect();

        fx->SetColor("MAT.Diffuse", mat->GetMaterialData().Diffuse);
        fx->SetColor("MAT.Ambient", mat->GetMaterialData().Ambient);
        fx->SetColor("MAT.Specular", mat->GetMaterialData().Specular);
        fx->SetColor("MAT.Emissive", mat->GetMaterialData().Emissive);
        fx->SetFloat("MAT.Power", mat->GetMaterialData().Power);
        fx->SetFloat("MAT.Opacity", mat->GetMaterialData().Opacity);

		fx->SetTexture("diffuseTex", mat->GetTextureHandle(TEXTURESLOT_ALBEDO));
		fx->SetBool("hasDiffuseTex", mat->GetTextureHandle(TEXTURESLOT_ALBEDO) != NULL);

		fx->SetTexture("specularTex", mat->GetTextureHandle(TEXTURESLOT_SPECULAR));
		fx->SetBool("hasSpecularTex", mat->GetTextureHandle(TEXTURESLOT_SPECULAR) != NULL);

        fx->SetTexture("normalTex", mat->GetTextureHandle(TEXTURESLOT_NORMAL));
        fx->SetBool("hasNormalTex", mat->GetTextureHandle(TEXTURESLOT_NORMAL) != NULL);

        fx->SetTexture("dispTex", mat->GetTextureHandle(TEXTURESLOT_DISPLACE));
        fx->SetBool("hasDispTex", mat->GetTextureHandle(TEXTURESLOT_DISPLACE) != NULL);
		fx->CommitChanges();
    }
	else if (GetActiveEffect())
	{
		CEffect* fx = GetActiveEffect();

        fx->SetTexture("diffuseTex", NULL);
        fx->SetBool("hasDiffuseTex", NULL);

        fx->SetTexture("specularTex", NULL);
        fx->SetBool("hasSpecularTex", NULL);

        fx->SetTexture("normalTex", NULL);
        fx->SetBool("hasNormalTex", NULL);

        fx->SetTexture("dispTex", NULL);
        fx->SetBool("hasDispTex", NULL);
	}
	else
	{
		SetTexture(stage, mat ? mat->GetTextureHandle() : NULL);
	}
}

VOID CRenderer::SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 handle)
{
    mDevice->SetTextureStageState(stage, D3DTSS_COLOROP, handle ? D3DTOP_SELECTARG1 : D3DTOP_SELECTARG2);
    mDevice->SetTexture(stage, handle);
}

VOID CRenderer::SetMatrix(UINT kind, const D3DXMATRIX& mat)
{
    mDevice->SetTransform((D3DTRANSFORMSTATETYPE)kind,
        &mat);

    if (kind == MATRIXKIND_PROJECTION || kind == MATRIXKIND_VIEW)
        GetFrustum()->Build();
}

VOID CRenderer::ResetMatrices()
{
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);

	mDevice->SetTransform(D3DTS_WORLD, &mat);
	mDevice->SetTransform(D3DTS_VIEW, &mat);
	mDevice->SetTransform(D3DTS_PROJECTION, &mat);
}

VOID CRenderer::SetRenderTarget(CRenderTarget* target, BOOL depth)
{
	if (target && target->GetSurfaceHandle())
	{
		mDevice->SetRenderTarget(0, target->GetSurfaceHandle());

        if (depth)
            mDevice->SetDepthStencilSurface(target->GetDepthSurfaceHandle());
	}
	else
	{
		mDevice->SetRenderTarget(0, mMainTarget->GetSurfaceHandle());
		mDevice->SetDepthStencilSurface(mMainTarget->GetDepthSurfaceHandle());
	}
}

VOID CRenderer::SetRenderState(DWORD kind, DWORD value)
{
	mDevice->SetRenderState((D3DRENDERSTATETYPE)kind, (DWORD)value);
}

VOID CRenderer::SetSamplerState(DWORD stage, DWORD kind, DWORD value)
{
	mDevice->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)kind, value);
}

VOID CRenderer::SetDefaultRenderStates()
{
    mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    mDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    mDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    mDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    mDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    mDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
}

D3DMATRIX CRenderer::GetDeviceMatrix(UINT kind)
{
	D3DMATRIX mat;

	mDevice->GetTransform((D3DTRANSFORMSTATETYPE)kind, &mat);
	return mat;
}

RECT CRenderer::GetLocalCoordinates(VOID) const
{
    RECT Rect;
    GetWindowRect(mWindow, &Rect);
    MapWindowPoints(HWND_DESKTOP, GetParent(mWindow), (LPPOINT)&Rect, 2);
    return Rect;
}

D3DVERTEXELEMENT9 meshVertexFormat[] =
{
    {0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,0},
    {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,0},
    {0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
    {0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
    {0, 48, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    {0, 52, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0},
    {0, 60, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,1},
    D3DDECL_END()
};

D3DVERTEXELEMENT9 meshVertex2DFormat[] =
{
    {0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT,0},
    {0, 16,  D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    {0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0},
    D3DDECL_END()
};
