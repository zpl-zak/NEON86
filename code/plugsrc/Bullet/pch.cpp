#include "pch.h"

// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.


#pragma comment(lib, "d3d9/d3dx9.lib")

#ifdef _DEBUG
#pragma comment(lib, "bullet/BulletCollision_Debug.lib")
#pragma comment(lib, "bullet/BulletDynamics_Debug.lib")
#pragma comment(lib, "bullet/BulletInverseDynamics_Debug.lib")
#pragma comment(lib, "bullet/BulletSoftBody_Debug.lib")
#pragma comment(lib, "bullet/LinearMath_Debug.lib")
#else
#pragma comment(lib, "bullet/BulletCollision.lib")
#pragma comment(lib, "bullet/BulletDynamics.lib")
#pragma comment(lib, "bullet/BulletInverseDynamics.lib")
#pragma comment(lib, "bullet/BulletSoftBody.lib")
#pragma comment(lib, "bullet/LinearMath.lib")
#endif
