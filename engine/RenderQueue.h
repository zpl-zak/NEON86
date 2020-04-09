#pragma once

#include "system.h"
#include "RenderCommand.h"

class ENGINE_API CRenderQueue
{
public:
	CRenderQueue(void);
	VOID Release(void);

	BOOL PushCommand(UINT kind, const RENDERDATA& data);
	
	VOID ExecutePreDraw(void);
	VOID ExecuteDraw(void);
	VOID ExecutePostDraw(void);

	VOID Clear(void);

private:
	CRenderCommand* mCommands;
	UINT mCapacity, mCount;
};