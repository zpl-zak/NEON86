// Renderer.cpp: implementation of the CRenderer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Renderer.h"
#include "RenderQueue.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderer::CRenderer()
{
	mRenderQueue = new CRenderQueue();
	mDirect9 = NULL;
	mDevice = NULL;
	mWindow = NULL;
	mVsync = TRUE;
	mCanAddCommands = FALSE;
}

VOID CRenderer::BuildParams()
{
	ZeroMemory(&mParams, sizeof(mParams));
	mParams.Windowed = TRUE;
	mParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	mParams.hDeviceWindow = mWindow;
	mParams.EnableAutoDepthStencil = TRUE;
	mParams.AutoDepthStencilFormat = D3DFMT_D16;

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

LRESULT CRenderer::CreateDevice(HWND window)
{
	mWindow = window;
	mDirect9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (mDirect9 == NULL)
		return ERROR_INVALID_HANDLE;

	BuildParams();

	LRESULT res = mDirect9->CreateDevice(	D3DADAPTER_DEFAULT,
							D3DDEVTYPE_HAL,
							window,
							D3DCREATE_HARDWARE_VERTEXPROCESSING,
							&mParams,
							&mDevice);

	if (!mDevice)
		return res;

	mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	mDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	mDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	mDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);

	return res;
}

VOID CRenderer::ResetDevice(void)
{
	if (!mDevice)
		return;

	BuildParams();
	mDevice->Reset(&mParams);
}

VOID CRenderer::Present()
{
	if (!mDevice)
		return;

	mRenderQueue->ExecutePreDraw();

	mDevice->BeginScene();
	mRenderQueue->ExecuteDraw();
	mDevice->EndScene();

	mRenderQueue->ExecutePostDraw();
	Clear();

	mDevice->Present(NULL, NULL, NULL, NULL);
}

VOID CRenderer::SetVSYNC(BOOL state)
{
	mVsync = state;
	ResetDevice();
}

VOID CRenderer::Clear()
{
	mRenderQueue->Clear();
}

VOID CRenderer::AllowCommands(BOOL state)
{
	mCanAddCommands = state;
}

BOOL CRenderer::Release()
{
	SAFE_RELEASE(mRenderQueue);
	SAFE_RELEASE(mDevice);
	SAFE_RELEASE(mDirect9);

	return TRUE;
}

void CRenderer::Resize(RECT res)
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

	mLastRes = res;
}

/// Render commands

VOID CRenderer::PushCommand(UINT kind, const RENDERDATA& data)
{
	if (!mCanAddCommands)
		return;

	mRenderQueue->PushCommand(kind, data);
}

VOID CRenderer::PushClear(D3DCOLOR color, UINT flags)
{
	RENDERDATA d;
	d.color = color;
	d.flags = flags;

	PushCommand(RENDERKIND_CLEAR, d);
}

VOID CRenderer::PushTexture(DWORD stage, LPDIRECT3DTEXTURE9 tex)
{
	RENDERDATA d;
	d.stage = stage;
	d.tex = tex;

	PushCommand(RENDERKIND_SET_TEXTURE, d);
}

VOID CRenderer::PushMatrix(UINT kind, const D3DXMATRIX& mat)
{
	RENDERDATA d;
	d.kind = kind;
	d.matrix = mat;

	PushCommand(RENDERKIND_MATRIX, d);
}

VOID CRenderer::PushPolygon(UINT kind, UINT primCount, const VERTEX* verts, UINT vertCount)
{
	if (!verts || vertCount > MAX_VERTS)
	{
		// todo: log
		return;
	}

	if (vertCount == 0)
		return;

	RENDERDATA d;
	d.kind = kind;
	d.vertCount = vertCount;
	d.primCount = primCount;
	memcpy(d.verts, verts, vertCount * sizeof(VERTEX));

	PushCommand(RENDERKIND_POLYGON, d);
}


VOID CRenderer::PushIndexedPolygon(UINT kind, UINT primCount, const SHORT* indices, UINT indexCount, const VERTEX* verts, UINT vertCount)
{
	if (!verts || vertCount > MAX_VERTS || !indices || indexCount > MAX_INDICES)
	{
		// todo: log
		return;
	}

	if (vertCount == 0 || indexCount == 0 || primCount == 0)
		return;

	RENDERDATA d;
	d.kind = kind;
	d.vertCount = vertCount;
	d.indexCount = indexCount;
	d.primCount = primCount;
	memcpy(d.verts, verts, vertCount * sizeof(VERTEX));
	memcpy(d.indices, indices, indexCount * sizeof(SHORT));

	PushCommand(RENDERKIND_POLYGON_INDEXED, d);
}