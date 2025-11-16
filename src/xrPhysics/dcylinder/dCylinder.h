
#ifndef dCylinder_h
#define dCylinder_h



#ifndef IXRAY_PHYSX
#include "../../3rd-party/ode/include/ode/common.h"
#endif

struct dxCylinder;
extern int dCylinderClassUser;


dxGeom *dCreateCylinder (dSpaceID space, dReal r, dReal lz);
void dGeomCylinderSetParams (dGeomID g, dReal radius, dReal length);

void dGeomCylinderGetParams (dGeomID g, dReal *radius, dReal *length);
#endif //dCylinder_h