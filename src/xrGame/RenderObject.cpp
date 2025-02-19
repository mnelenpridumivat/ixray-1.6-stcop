#include "StdAfx.h"
#include "RenderObject.h"
#include "../Include/xrRender/RenderVisual.h"
#include "../Include/xrRender/Kinematics.h"
#include "../Include/xrRender/KinematicsAnimated.h"

IRenderObject::IRenderObject(IRenderVisual* Obj)
{
	VERIFY(Obj->dcast_PKinematics());
	VERIFY(Obj->dcast_PKinematicsAnimated());
	ptr = Obj;
}

IRenderObject::IRenderObject(IKinematics* Obj)
{
	VERIFY(Obj->dcast_RenderVisual());
	VERIFY(Obj->dcast_PKinematicsAnimated());
	ptr = Obj;
}

IRenderObject::IRenderObject(IKinematicsAnimated* Obj)
{
	VERIFY(Obj->dcast_PKinematics());
	VERIFY(Obj->dcast_RenderVisual());
	ptr = Obj;
}

IRenderObject& IRenderObject::operator=(IRenderVisual* Obj)
{
	VERIFY(Obj->dcast_PKinematics());
	VERIFY(Obj->dcast_PKinematicsAnimated());
	ptr = Obj;
	return *this;
}

IRenderObject& IRenderObject::operator=(IKinematics* Obj)
{
	VERIFY(Obj->dcast_RenderVisual());
	VERIFY(Obj->dcast_PKinematicsAnimated());
	ptr = Obj;
	return *this;
}

IRenderObject& IRenderObject::operator=(IKinematicsAnimated* Obj)
{
	VERIFY(Obj->dcast_PKinematics());
	VERIFY(Obj->dcast_RenderVisual());
	ptr = Obj;
	return *this;
}
