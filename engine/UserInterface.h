#pragma once

#include "system.h"

#define UI CEngine::the()->GetUI()

class ENGINE_API CUserInterface
{
public:
    CUserInterface();
    BOOL Release(VOID);
};

