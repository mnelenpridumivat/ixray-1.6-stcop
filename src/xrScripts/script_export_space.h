////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export_space.h
//	Created 	: 22.09.2003
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script export space 
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "linker.h"
#include "../xrCore/_stl_extensions.h"
#include "../xrCore/_types.h"
#include <concepts>

struct lua_State;

#define DECLARE_SCRIPT_REGISTER_FUNCTION public: static void script_register(lua_State *);
#define DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT static void script_register(lua_State *);

template <typename T> struct enum_exporter{DECLARE_SCRIPT_REGISTER_FUNCTION};
template <typename T> struct class_exporter{DECLARE_SCRIPT_REGISTER_FUNCTION};

template <typename T> struct SCRIPTS_API enum_exporter_lib { DECLARE_SCRIPT_REGISTER_FUNCTION };
template <typename T> struct SCRIPTS_API class_exporter_lib { DECLARE_SCRIPT_REGISTER_FUNCTION };

template<typename T>
concept has_script_register =
requires(lua_State* a) {
	{ T::script_register(a) } -> std::same_as<void>;
};

class SCRIPTS_API script_exporter_key_base {
	friend class std::hash<script_exporter_key_base>;

protected:
	shared_str id;

public:
	script_exporter_key_base(shared_str name) {
		id = name;
	}
	script_exporter_key_base() = delete;
	~script_exporter_key_base(){}

	bool operator==(const script_exporter_key_base& other) const {
		return id == other.id;
	}

	bool operator==(script_exporter_key_base&& other) const {
		return id == other.id;
	}

	bool operator!=(const script_exporter_key_base& other) const {
		return id != other.id;
	}

	bool operator!=(script_exporter_key_base&& other) const {
		return id != other.id;
	}
};

template <>
struct std::hash<script_exporter_key_base>
{
	std::size_t operator()(const script_exporter_key_base& k) const
	{
		return std::hash<shared_str>()(k.id);
	}
};

template<has_script_register T>
class script_exporter_key : public script_exporter_key_base {
public:
	static const script_exporter_key_base GetKey(shared_str name) {
		static script_exporter_key_base key(name);
		return key;
	}
};

class script_exporter_base {
public:
	virtual void script_register(lua_State* L) = 0;
};

struct script_exporter_data {
	bool inited = false;
	xr_vector<script_exporter_key_base> dependencies;
	script_exporter_base* exporter = nullptr;
};

SCRIPTS_API xr_hash_map<script_exporter_key_base, script_exporter_data*>& get_script_export_container();

#define SCRIPT_EXPORT1(x) script_exporter1<x> x##_exporter = script_exporter1<x>::GetInstance(#x);
#define SCRIPT_EXPORT2(x, x1) script_exporter2<x, x1> x##_exporter = script_exporter2<x, x1>::GetInstance(#x, #x1);
#define SCRIPT_EXPORT3(x, x1, x2) script_exporter3<x, x1, x2> x##_exporter = script_exporter3<x, x1, x2>::GetInstance(#x, #x1, #x2);
#define SCRIPT_EXPORT4(x, x1, x2, x3) script_exporter4<x, x1, x2, x3> x##_exporter = script_exporter4<x, x1, x2, x3>::GetInstance(#x, #x1, #x2, #x3);
#define SCRIPT_EXPORT5(x, x1, x2, x3, x4) script_exporter5<x, x1, x2, x3, x4> x##_exporter = script_exporter5<x, x1, x2, x3, x4>::GetInstance(#x, #x1, #x2, #x3, #x4);
#define SCRIPT_EXPORT_NAME1(x, name) script_exporter1<x> name##_exporter = script_exporter1<x>::GetInstance(#x);
#define SCRIPT_EXPORT_NAME2(x, name, x1) script_exporter2<x, x1> name##_exporter = script_exporter2<x, x1>::GetInstance(#x, #x1);

template<has_script_register T>
class script_exporter1 : public script_exporter_base {

	script_exporter1(shared_str vT) {
		script_exporter_data* data = new script_exporter_data();
		data->inited = false;
		data->exporter = this;
		data->dependencies = {};
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(vT), data });
	}

public:
	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter1<T> GetInstance(shared_str vT) {
		static script_exporter1<T> instance(vT);
		return instance;
	}
};

template<has_script_register T, has_script_register A1>
class script_exporter2 : public script_exporter_base {

	script_exporter2(shared_str vT, shared_str vA1) {
		script_exporter_data* data = new script_exporter_data();
		data->inited = false;
		data->exporter = this;
		data->dependencies = {};
		data->dependencies.push_back(script_exporter_key<A1>::GetKey(vA1));
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(vT), data });
	}

public:
	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter2<T, A1> GetInstance(shared_str vT, shared_str vA1) {
		static script_exporter2<T, A1> instance(vT, vA1);
		return instance;
	}
};

template<has_script_register T, has_script_register A1, has_script_register A2>
class script_exporter3 : public script_exporter_base {

	script_exporter3(shared_str vT, shared_str vA1, shared_str vA2) {
		script_exporter_data* data = new script_exporter_data();
		data->inited = false;
		data->exporter = this;
		data->dependencies = {};
		data->dependencies.push_back(script_exporter_key<A1>::GetKey(vA1));
		data->dependencies.push_back(script_exporter_key<A2>::GetKey(vA2));
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(vT), data });
	}

public:
	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter3<T, A1, A2> GetInstance(shared_str vT, shared_str vA1, shared_str vA2) {
		static script_exporter3<T, A1, A2> instance(vT, vA1, vA2);
		return instance;
	}
};

template<has_script_register T, has_script_register A1, has_script_register A2, has_script_register A3>
class script_exporter4 : public script_exporter_base {

	script_exporter4(shared_str vT, shared_str vA1, shared_str vA2, shared_str vA3) {
		script_exporter_data* data = new script_exporter_data();
		data->inited = false;
		data->exporter = this;
		data->dependencies = {};
		data->dependencies.push_back(script_exporter_key<A1>::GetKey(vA1));
		data->dependencies.push_back(script_exporter_key<A2>::GetKey(vA2));
		data->dependencies.push_back(script_exporter_key<A3>::GetKey(vA3));
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(vT), data });
	}

public:
	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter4<T, A1, A2, A3> GetInstance(shared_str vT, shared_str vA1, shared_str vA2, shared_str vA3) {
		static script_exporter4<T, A1, A2, A3> instance(vT, vA1, vA2, vA3);
		return instance;
	}
};

template<has_script_register T, has_script_register A1, has_script_register A2, has_script_register A3, has_script_register A4>
class script_exporter5 : public script_exporter_base {

	script_exporter5(shared_str vT, shared_str vA1, shared_str vA2, shared_str vA3, shared_str vA4) {
		script_exporter_data* data = new script_exporter_data();
		data->inited = false;
		data->exporter = this;
		data->dependencies = {};
		data->dependencies.push_back(script_exporter_key<A1>::GetKey(vA1));
		data->dependencies.push_back(script_exporter_key<A2>::GetKey(vA2));
		data->dependencies.push_back(script_exporter_key<A3>::GetKey(vA3));
		data->dependencies.push_back(script_exporter_key<A4>::GetKey(vA4));
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(vT), data });
	}

public:
	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter5<T, A1, A2, A3, A4> GetInstance(shared_str vT, shared_str vA1, shared_str vA2, shared_str vA3, shared_str vA4) {
		static script_exporter5<T, A1, A2, A3, A4> instance(vT, vA1, vA2, vA3, vA4);
		return instance;
	}
};
