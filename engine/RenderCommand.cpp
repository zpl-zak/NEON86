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
	switch (mKind)
	{		
	case RENDERKIND_MATRIX:
		{
			RENDERER->GetDevice()->SetTransform((D3DTRANSFORMSTATETYPE)mData.kind,
												&mData.matrix);
		}
		break;
	case RENDERKIND_POLYGON:
		{
			LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
			LPDIRECT3DVERTEXBUFFER9 buffer = RENDERER->GetVertexBuffer();
			VOID* vidMem;

			buffer->Lock(0,
				mData.vertCount * sizeof(VERTEX),
				(VOID**)&vidMem,
				0);

			memcpy(vidMem, mData.verts, mData.vertCount*sizeof(VERTEX));
			buffer->Unlock();
			dev->SetFVF(NEONFVF);
			dev->SetStreamSource(0,
								 buffer,
								 0,
								 sizeof(VERTEX));
			dev->SetIndices(NULL);

			dev->DrawPrimitive((D3DPRIMITIVETYPE)mData.kind, 0, mData.primCount);
		}
		break;
	case RENDERKIND_POLYGON_INDEXED:
		{
			LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
			LPDIRECT3DVERTEXBUFFER9 buffer = RENDERER->GetVertexBuffer();
			LPDIRECT3DINDEXBUFFER9 indexBuffer = RENDERER->GetIndexBuffer();
			VOID* vidMem, *indexVidMem;

			buffer->Lock(0,
				mData.vertCount * sizeof(VERTEX),
				(VOID**)&vidMem,
				0);

			memcpy(vidMem, mData.verts, mData.vertCount*sizeof(VERTEX));
			buffer->Unlock();

			indexBuffer->Lock(0,
							  mData.indexCount * sizeof(SHORT),
							  (VOID**)&indexVidMem,
							  0);

			memcpy(indexVidMem, mData.indices, mData.indexCount*sizeof(SHORT));
			indexBuffer->Unlock();

			dev->SetFVF(NEONFVF);
			dev->SetStreamSource(0,
				buffer,
				0,
				sizeof(VERTEX));
			dev->SetIndices(indexBuffer);

			dev->DrawIndexedPrimitive((D3DPRIMITIVETYPE)mData.kind, 0, 0, mData.vertCount, 0, mData.primCount);
		}
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