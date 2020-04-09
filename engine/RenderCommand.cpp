#include "stdafx.h"

#include "RenderCommand.h"
#include "NeonEngine.h"

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
		}
		break;
	case RENDERKIND_POLYGON:
		{
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
	case RENDERKIND_SET_TEXTURE:
		dev->SetTextureStageState(mData.stage, D3DTSS_COLOROP, mData.tex ? D3DTOP_SELECTARG1 : D3DTOP_SELECTARG2);
		dev->SetTexture(mData.stage, mData.tex);
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