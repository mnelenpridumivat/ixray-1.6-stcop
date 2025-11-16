#ifndef PH_BASE_BODY_EFFECTOR_H
#define PH_BASE_BODY_EFFECTOR_H

//#include "ode_include.h"
#ifndef IXRAY_PHYSX
#include "../3rd-party/ode/include/ode/common.h"
#endif
class CPHBaseBodyEffector 
{
protected:
	dBodyID m_body;
public:
	void Init(dBodyID body)
		{
			m_body=body;
		}
};
#endif