#pragma once

#include "system.h"

#include "RenderCommand.h"

#include <d3d9.h>

class ENGINE_API CClearScreenCommand : public CRenderCommand  
{
public:
	CClearScreenCommand(D3DCOLOR color, UINT flags);
	virtual ~CClearScreenCommand();
	
	virtual BOOL ExecuteCommand(void);

private:
	D3DCOLOR mColor;
	UINT mFlags;
};
