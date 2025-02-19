#pragma once

class IRenderVisual;
class IKinematics;
class IKinematicsAnimated;

class IRenderObject {
	void* ptr = nullptr;
public:

	IRenderObject() {};
	IRenderObject(IRenderVisual* Obj);
	IRenderObject(IKinematics* Obj);
	IRenderObject(IKinematicsAnimated* Obj);

	IRenderObject& operator=(IRenderVisual* Obj);
	IRenderObject& operator=(IKinematics* Obj);
	IRenderObject& operator=(IKinematicsAnimated* Obj);

	FORCEINLINE void Reset() { ptr = nullptr; }
	FORCEINLINE bool IsValid() { return ptr; }
	FORCEINLINE IRenderVisual* AsRV() { return (IRenderVisual*)ptr; }
	FORCEINLINE IKinematics* AsK() { return (IKinematics*)ptr; }
	FORCEINLINE IKinematicsAnimated* AsKA() { return (IKinematicsAnimated*)ptr; }

};