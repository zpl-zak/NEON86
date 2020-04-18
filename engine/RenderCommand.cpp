#include "stdafx.h"

#include "RenderCommand.h"
#include "NeonEngine.h"

#include "Texture.h"
#include "Frustum.h"

CRenderCommand::CRenderCommand(UINT kind, RENDERDATA data)
{
	mKind = kind;
	mData = data;
}

CRenderCommand::~CRenderCommand()
{

}

void CRenderCommand::ExecutePreDraw(void)
{
	switch (mKind)
	{
	case RENDERKIND_CLEAR:
		RENDERER->GetDevice()->Clear(0, NULL, mData.flags, mData.color, 1.0f, 0);
		break;

	default:
		break;
	}
}

void CRenderCommand::ExecuteDraw(void)
{
	LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();

	switch (mKind)
	{		
	case RENDERKIND_MATRIX:
		{
			dev->SetTransform((D3DTRANSFORMSTATETYPE)mData.kind,
												&mData.matrix);
			
			if (mData.kind == MATRIXKIND_PROJECTION || mData.kind == MATRIXKIND_VIEW)
				RENDERER->GetFrustum()->Build();
		}
		break;
	case RENDERKIND_POLYGON:
		{
			D3DXMATRIX wmat;
			if (mData.usesMatrix)
				wmat = mData.matrix;
			else dev->GetTransform(D3DTS_WORLD, &wmat);
			
			// Check frustum
			// TODO use sphere/AABB check
			D3DXVECTOR3 wpos = D3DXVECTOR3(wmat._41, wmat._42, wmat._43);
			

			if (!RENDERER->GetFrustum()->IsPointVisible(wpos))
				break;

			if (mData.usesMatrix)
				dev->SetTransform(D3DTS_WORLD, &mData.matrix);

			dev->SetFVF(NEONFVF);
			dev->SetStreamSource(0,
				mData.vertBuffer,
				0,
				sizeof(VERTEX));
			dev->SetIndices(mData.indexBuffer);

			if (mData.indexBuffer)
				dev->DrawIndexedPrimitive((D3DPRIMITIVETYPE)mData.kind, 0, 0, mData.vertCount, 0, mData.primCount);
			else
				dev->DrawPrimitive((D3DPRIMITIVETYPE)mData.kind, 0, mData.primCount);
		}
		break;
	case RENDERKIND_QUAD:
	{
		// todo
	} break;
	case RENDERKIND_SET_TEXTURE:
		dev->SetTextureStageState(mData.stage, D3DTSS_COLOROP, mData.tex ? D3DTOP_MODULATE : D3DTOP_SELECTARG2);
		dev->SetTexture(mData.stage, mData.tex ? mData.tex->GetTextureHandle() : NULL);
		break;
	case RENDERKIND_SET_SAMPLERSTATE:
		dev->SetSamplerState(mData.stage, (D3DSAMPLERSTATETYPE)mData.kind, mData.state);
		break;
	case RENDERKIND_SET_RENDERSTATE:
		dev->SetRenderState((D3DRENDERSTATETYPE)mData.kind, (DWORD)mData.state);
		break;
	default:
		break;
	}
}

void CRenderCommand::ExecutePostDraw(void)
{
	/*switch (mKind)
	{
	default:
		break;
	}*/
}