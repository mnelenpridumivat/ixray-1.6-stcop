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

struct lua_State;

#define DECLARE_SCRIPT_REGISTER_FUNCTION public: static void script_register(lua_State *);
#define DECLARE_SCRIPT_REGISTER_FUNCTION_STRUCT static void script_register(lua_State *);

template <typename T> struct enum_exporter{DECLARE_SCRIPT_REGISTER_FUNCTION};
template <typename T> struct class_exporter{DECLARE_SCRIPT_REGISTER_FUNCTION};

template <typename T> struct SCRIPTS_API enum_exporter_lib { DECLARE_SCRIPT_REGISTER_FUNCTION };
template <typename T> struct SCRIPTS_API class_exporter_lib { DECLARE_SCRIPT_REGISTER_FUNCTION };

struct script_exporter_key_base {

};

template<typename T>
struct script_exporter_key : public script_exporter_key_base {
	static script_exporter_key* GetKey() {
		static script_exporter_key<T> key;
		return &key;
	}
};

struct script_exporter_base {

	virtual void script_register(lua_State* L) = 0;
};

struct script_exporter_data {
	bool inited = false;
	xr_vector<script_exporter_key_base*> dependencies;
	script_exporter_base* exporter = nullptr;
};

SCRIPTS_API xr_hash_map<script_exporter_key_base*, script_exporter_data>& get_script_export_container();

#define SCRIPT_EXPORT1(x) script_exporter1<x> x##_exporter = script_exporter1<x>::GetInstance();
#define SCRIPT_EXPORT2(x, x1) script_exporter2<x, x1> x##_exporter = script_exporter2<x, x1>::GetInstance();
#define SCRIPT_EXPORT3(x, x1, x2) script_exporter3<x, x1, x2> x##_exporter = script_exporter3<x, x1, x2>::GetInstance();
#define SCRIPT_EXPORT4(x, x1, x2, x3) script_exporter4<x, x1, x2, x3> x##_exporter = script_exporter4<x, x1, x2, x3>::GetInstance();
#define SCRIPT_EXPORT5(x, x1, x2, x3, x4) script_exporter5<x, x1, x2, x3, x4> x##_exporter = script_exporter5<x, x1, x2, x3, x4>::GetInstance();
#define SCRIPT_EXPORT_NAME1(x, name) script_exporter1<x> name##_exporter = script_exporter1<x>::GetInstance();
#define SCRIPT_EXPORT_NAME2(x, name, x1) script_exporter2<x, x1> name##_exporter = script_exporter2<x, x1>::GetInstance();

template <typename T>
class has_script_register
{
	typedef char one;
	struct two { char x[2]; };

	template <typename C> static one test(decltype(&C::script_register));
	template <typename C> static two test(...);

public:
	enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template<typename T>
struct script_exporter1 : public script_exporter_base {

	script_exporter1() {
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(), {} });
		auto& value = get_script_export_container()[script_exporter_key<T>::GetKey()];
		value.inited = false;
		value.exporter = this;
	}

	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter1<T>& GetInstance() {
		static script_exporter1<T> instance;
		return instance;
	}
};

template<typename T, typename A1>
struct script_exporter2 : public script_exporter_base {

	script_exporter2() {
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(), {} });
		auto& value = get_script_export_container()[script_exporter_key<T>::GetKey()];
		value.inited = false;
		static_assert(has_script_register<A1>::value);
		value.dependencies.push_back(script_exporter_key<A1>::GetKey());
		value.exporter = this;
	}

	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter2<T, A1>& GetInstance() {
		static script_exporter2<T, A1> instance;
		return instance;
	}
};

template<typename T, typename A1, typename A2>
struct script_exporter3 : public script_exporter_base {

	script_exporter3() {
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(), {} });
		auto& value = get_script_export_container()[script_exporter_key<T>::GetKey()];
		value.inited = false;
		static_assert(has_script_register<A1>::value);
		value.dependencies.push_back(script_exporter_key<A1>::GetKey());
		static_assert(has_script_register<A2>::value);
		value.dependencies.push_back(script_exporter_key<A2>::GetKey());
		value.exporter = this;
	}

	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter3<T, A1, A2>& GetInstance() {
		static script_exporter3<T, A1, A2> instance;
		return instance;
	}
};

template<typename T, typename A1, typename A2, typename A3>
struct script_exporter4 : public script_exporter_base {

	script_exporter4() {
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(), {} });
		auto& value = get_script_export_container()[script_exporter_key<T>::GetKey()];
		value.inited = false;
		static_assert(has_script_register<A1>::value);
		value.dependencies.push_back(script_exporter_key<A1>::GetKey());
		static_assert(has_script_register<A2>::value);
		value.dependencies.push_back(script_exporter_key<A2>::GetKey());
		static_assert(has_script_register<A3>::value);
		value.dependencies.push_back(script_exporter_key<A3>::GetKey());
		value.exporter = this;
	}

	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter4<T, A1, A2, A3>& GetInstance() {
		static script_exporter4<T, A1, A2, A3> instance;
		return instance;
	}
};

template<typename T, typename A1, typename A2, typename A3, typename A4>
struct script_exporter5 : public script_exporter_base {

	script_exporter5() {
		get_script_export_container().insert({ script_exporter_key<T>::GetKey(), {} });
		auto& value = get_script_export_container()[script_exporter_key<T>::GetKey()];
		value.inited = false;
		static_assert(has_script_register<A1>::value);
		value.dependencies.push_back(script_exporter_key<A1>::GetKey());
		static_assert(has_script_register<A2>::value);
		value.dependencies.push_back(script_exporter_key<A2>::GetKey());
		static_assert(has_script_register<A3>::value);
		value.dependencies.push_back(script_exporter_key<A3>::GetKey());
		static_assert(has_script_register<A4>::value);
		value.dependencies.push_back(script_exporter_key<A4>::GetKey());
		value.exporter = this;
	}

	virtual void script_register(lua_State* L) override {
		T::script_register(L);
	}

	static script_exporter5<T, A1, A2, A3, A4>& GetInstance() {
		static script_exporter5<T, A1, A2, A3, A4> instance;
		return instance;
	}
};
