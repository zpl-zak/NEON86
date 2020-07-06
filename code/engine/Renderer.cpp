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
	mDefaultBox = NULL;
	mDefaultMaterial = NULL;
	mVsync = TRUE;
	mFullscreen = FALSE;
	mEnableLighting = FALSE;
	mUsesMaterialOverride = FALSE;
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

VOID CRenderer::PrepareEffectDraw()
{
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

		D3DCOLOR ambience = 0x0;
		GetDevice()->GetRenderState(D3DRS_AMBIENT, &ambience);
		GetActiveEffect()->SetInteger("NEON.AmbientColor", ambience);

        GetActiveEffect()->CommitChanges();
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

	LPDIRECT3DSURFACE9 display;
	mDevice->GetRenderTarget(0, &display);
	display->GetDesc(&mDisplayDesc);
	display->Release();

	winres.right = mDisplayDesc.Width;
	winres.bottom = mDisplayDesc.Height;

	Resize(winres);

	mMainTarget = new CRenderTarget(winres.right, winres.bottom, TRUE);
	SetRenderTarget(NULL);
    mDefaultMaterial = new CMaterial();

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

VOID CRenderer::BeginRender(VOID)
{
    SetDefaultRenderStates();
    GetDevice()->BeginScene();
}

VOID CRenderer::EndRender(VOID)
{
    Blit();
    GetDevice()->EndScene();
    GetDevice()->Present(NULL, NULL, NULL, NULL);
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
	SAFE_RELEASE(mDefaultMaterial);

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
	D3DMATRIX oldMatrix;
	if (data.usesMatrix)
	{
		oldMatrix = GetDeviceMatrix(MATRIXKIND_WORLD);
		SetMatrix(MATRIXKIND_WORLD, data.matrix);
	}

	PrepareEffectDraw();

	if (data.mesh)
		data.mesh->DrawSubset(0);

    if (data.usesMatrix)
    {
        SetMatrix(MATRIXKIND_WORLD, oldMatrix);
    }
}

VOID CRenderer::DrawPolygon(VERTEX a, VERTEX b, VERTEX c)
{
	ZeroMemory(&mImmediateBuffer, sizeof(mImmediateBuffer));
	mImmediateBuffer[0] = { a.x, a.y, a.z, 0,0,0,0,0,0,0,0,0, a.color, a.su, a.tv };
	mImmediateBuffer[1] = { b.x, b.y, b.z, 0,0,0,0,0,0,0,0,0, b.color, b.su, b.tv };
	mImmediateBuffer[2] = {c.x, c.y, c.z, 0,0,0,0,0,0,0,0,0, c.color, c.su, c.tv};

    static IDirect3DVertexDeclaration9* vertsDecl = NULL;

    if (!vertsDecl)
        mDevice->CreateVertexDeclaration(meshVertexFormat, &vertsDecl);

    mDevice->SetVertexDeclaration(vertsDecl);
    mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 3, (LPVOID)mImmediateBuffer, sizeof(VERTEX));
}

VOID CRenderer::DrawQuad3D(FLOAT x1, FLOAT x2, FLOAT y1, FLOAT y2, FLOAT z1, FLOAT z2, DWORD color)
{
    VERTEX verts[] =
    {
        {x1, y2, z2, 0,0,0,0,0,0,0,0,0,  color, 0.0f, 0.0f},
        {x2, y1, z1, 0,0,0,0,0,0,0,0,0,  color, 1.0f, 1.0f},
        {x2, y2, z2, 0,0,0,0,0,0,0,0,0,  color, 1.0f, 0.0f},

        {x1, y2, z2, 0,0,0,0,0,0,0,0,0,  color, 0.0f, 0.0f},
        {x1, y1, z1, 0,0,0,0,0,0,0,0,0,  color, 0.0f, 1.0f},
        {x2, y1, z1, 0,0,0,0,0,0,0,0,0,  color, 1.0f, 1.0f},
    };

    static IDirect3DVertexDeclaration9* vertsDecl = NULL;

    if (!vertsDecl)
        mDevice->CreateVertexDeclaration(meshVertexFormat, &vertsDecl);

    mDevice->SetVertexDeclaration(vertsDecl);
    mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 6, (LPVOID)verts, sizeof(VERTEX));
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

VOID CRenderer::DrawBox(D3DXMATRIX mat, D3DXVECTOR4 dims, DWORD color)
{
	SAFE_RELEASE(mDefaultBox);
	
    SetMatrix(MATRIXKIND_WORLD, mat);

	D3DXCreateBox(mDevice, ::abs(dims.x), ::abs(dims.y), ::abs(dims.z), &mDefaultBox, NULL);

	if (!mDefaultBox)
		return;

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

VOID CRenderer::ClearBuffer(D3DCOLOR color, UINT flags)
{
	mDevice->Clear(0, NULL, flags, color, 1.0f, 1);
}

VOID CRenderer::SetMaterial(DWORD stage, CMaterial* mat)
{
    if (GetActiveEffect() && mat)
    {
        CEffect* fx = GetActiveEffect();
		MATERIAL matData = mat->GetMaterialData();

        fx->SetColor("MAT.Diffuse", matData.Diffuse);
        fx->SetColor("MAT.Ambient", matData.Ambient);
        fx->SetColor("MAT.Specular", matData.Specular);
        fx->SetColor("MAT.Emissive", matData.Emissive);
        fx->SetFloat("MAT.Power", matData.Power);
        fx->SetFloat("MAT.Opacity", matData.Opacity);
		fx->SetBool("MAT.IsShaded", matData.Shaded);
		fx->SetBool("MAT.AlphaIsTransparency", matData.AlphaIsTransparency);

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
		MATERIAL* matData = mat ? &mat->GetMaterialData() : NULL;
		SetTexture(stage, mat ? mat->GetTextureHandle() : NULL);
		if (mat) mDevice->SetMaterial(matData);
		else {
			mDevice->SetMaterial(&mDefaultMaterial->GetMaterialData());
		}

		if (mat && mat->IsTransparent())
		{
            mDevice->SetTextureStageState(stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            mDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            mDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG2, D3DTA_CONSTANT);

            DWORD alphaColor = D3DCOLOR_ARGB(matData ? (DWORD)(matData->Opacity * 255.0f) : 255, 255, 255, 255);
            mDevice->SetTextureStageState(stage, D3DTSS_CONSTANT, alphaColor);

            mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

			if (matData && matData->AlphaIsTransparency == TRUE && matData->AlphaTestEnabled == TRUE)
			{
				mDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)matData->AlphaRef);
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

VOID CRenderer::SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 handle)
{
    mDevice->SetTextureStageState(stage, D3DTSS_COLOROP, handle ? D3DTOP_MODULATE : D3DTOP_SELECTARG2);
    mDevice->SetTexture(stage, handle);
}

VOID CRenderer::SetMatrix(UINT kind, const D3DXMATRIX& mat)
{
    mDevice->SetTransform((D3DTRANSFORMSTATETYPE)kind, &mat);

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

VOID CRenderer::SetFog(DWORD color, DWORD mode, FLOAT start, FLOAT end)
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

VOID CRenderer::ClearFog()
{
	mDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

VOID CRenderer::SetDefaultRenderStates()
{
	mDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	mDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	mDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	mDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	
    mDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)IsLightingEnabled());
	mDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	mDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	mDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	mDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

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
