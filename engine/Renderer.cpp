// Renderer.cpp: implementation of the CRenderer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Renderer.h"
#include "Texture.h"
#include "Frustum.h"
#include "Mesh.h"

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

    mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

VOID CRenderer::SetVSYNC(BOOL state)
{
	mVsync = state;
	ResetDevice();
}

VOID CRenderer::Clear()
{

}

VOID CRenderer::AllowCommands(BOOL state)
{
	mCanAddCommands = state;
}

BOOL CRenderer::Release()
{
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
VOID CRenderer::PushPolygon(const RENDERDATA& data)
{
    D3DXMATRIX wmat;
    if (data.usesMatrix)
        wmat = data.matrix;
    else mDevice->GetTransform(D3DTS_WORLD, &wmat);

    // Check frustum
    // TODO use sphere/AABB check
    D3DXVECTOR3 wpos = D3DXVECTOR3(wmat._41, wmat._42, wmat._43);
	D3DXVECTOR3 pos = data.meshOrigin + wpos;

	if (!GetFrustum()->IsSphereVisible(pos, data.meshRadius))
		return;

    if (data.usesMatrix)
        mDevice->SetTransform(D3DTS_WORLD, &data.matrix);

	data.mesh->DrawSubset(0);
}

VOID CRenderer::PushClear(D3DCOLOR color, UINT flags)
{
	mDevice->Clear(0, NULL, flags, color, 1.0f, 0);
}

VOID CRenderer::PushTexture(DWORD stage, CTexture* tex)
{
    mDevice->SetTextureStageState(stage, D3DTSS_COLOROP, tex ? D3DTOP_MODULATE : D3DTOP_SELECTARG2);
    mDevice->SetTexture(stage, tex ? tex->GetTextureHandle() : NULL);
}

VOID CRenderer::PushMatrix(UINT kind, const D3DXMATRIX& mat)
{
    mDevice->SetTransform((D3DTRANSFORMSTATETYPE)kind,
        &mat);

    if (kind == MATRIXKIND_PROJECTION || kind == MATRIXKIND_VIEW)
        GetFrustum()->Build();
}

VOID CRenderer::PushRenderState(DWORD kind, BOOL state)
{
	mDevice->SetRenderState((D3DRENDERSTATETYPE)kind, (DWORD)state);
}

VOID CRenderer::PushSamplerState(DWORD stage, DWORD kind, DWORD value)
{
	mDevice->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)kind, value);
}

D3DMATRIX CRenderer::GetDeviceMatrix(UINT kind)
{
	D3DMATRIX mat;

	mDevice->GetTransform((D3DTRANSFORMSTATETYPE)kind, &mat);
	return mat;
}
