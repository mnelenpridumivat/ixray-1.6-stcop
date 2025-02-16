#pragma once
#include "../script_export_space.h"

class ISaveObject;

/*class SCRIPTS_API ISaveObjectWrapper {
	ISaveObject* Obj;
public:

	ISaveObject& GetObj();
	ISaveObjectWrapper(ISaveObject* Obj) : Obj(Obj) {}
	void BeginChunk(LPCSTR Name);
	void EndChunk();
	void BeginArray();
	void EndArray();
	void s_vec3(luabind::object Value);
	void s_float(luabind::object Value);
	void s_u64(luabind::object Value);
	void s_s64(luabind::object Value);
	void s_u32(luabind::object Value);
	void s_s32(luabind::object Value);
	void s_u16(luabind::object Value);
	void s_s16(luabind::object Value);
	void s_u8(luabind::object Value);
	void s_s8(luabind::object Value);
	void s_bool(luabind::object Value);
	void s_stringZ(luabind::object Value);
	bool IsSave();
};*/

class SCRIPTS_API SaveSystemScript {
	DECLARE_SCRIPT_REGISTER_FUNCTION
};