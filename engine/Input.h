#pragma once

#include "system.h"

#define INPUT CEngine::the()->GetInput()

class ENGINE_API CInput
{
public:
	CInput(void);
	
	VOID Release(void);
};
