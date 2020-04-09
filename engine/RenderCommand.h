#pragma once

#include "system.h"
#include "RenderData.h"

class ENGINE_API CRenderCommand
{
public:
	CRenderCommand() {};
	CRenderCommand(UINT, RENDERDATA);
	~CRenderCommand();

	VOID ExecutePreDraw(void);
	VOID ExecuteDraw(void);
	VOID ExecutePostDraw(void);

private:
	UINT mKind;
	RENDERDATA mData;
};
