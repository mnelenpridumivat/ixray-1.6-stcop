#include "stdafx.h"
#pragma hdrstop

#include "PHNetState.h"
#include "fastdelegate.h"
#include "Save/SaveObject.h"

ISaveObject& operator<<(ISaveObject& Object, SPHNetState& Value) {
	Value.net_Serialize(Object);
	return Object;
}

//////////////////////////////////////8/////////////////////////////////////////////////////

static void w_vec_q8(NET_Packet& P,const Fvector& vec,const Fvector& min,const Fvector& max)
{
	P.w_float_q8(vec.x,min.x,max.x);
	P.w_float_q8(vec.y,min.y,max.y);
	P.w_float_q8(vec.z,min.z,max.z);
}
/*static void w_vec_q8(CSaveObject& P, const Fvector& vec, const Fvector& min, const Fvector& max)
{
	P->w_float(vec.x);
	P->w_float(vec.y);
	P->w_float(vec.z);
}*/
template<typename src>
static void r_vec_q8(src& P,Fvector& vec,const Fvector& min,const Fvector& max)
{
	vec.x=P.r_float_q8(min.x,max.x);
	vec.y=P.r_float_q8(min.y,max.y);
	vec.z=P.r_float_q8(min.z,max.z);

	clamp(vec.x,min.x,max.x);
	clamp(vec.y,min.y,max.y);
	clamp(vec.z,min.z,max.z);
}
/*static void r_vec_q8(CSaveChunk* P, Fvector& vec, const Fvector& min, const Fvector& max)
{
	P->r_float(vec.x);
	P->r_float(vec.y);
	P->r_float(vec.z);

	clamp(vec.x, min.x, max.x);
	clamp(vec.y, min.y, max.y);
	clamp(vec.z, min.z, max.z);
}*/
static void s_vec_q8(ISaveObject& P, Fvector& vec, const Fvector& min, const Fvector& max)
{
	if (P.IsSave()) {
		clamp(vec.x, min.x, max.x);
		clamp(vec.y, min.y, max.y);
		clamp(vec.z, min.z, max.z);
	}
	P << vec;
	if (!P.IsSave()) {
		clamp(vec.x, min.x, max.x);
		clamp(vec.y, min.y, max.y);
		clamp(vec.z, min.z, max.z);
	}
}

static void w_qt_q8(NET_Packet& P,const Fquaternion& q)
{
	P.w_float_q8(q.x,-1.f,1.f);
	P.w_float_q8(q.y,-1.f,1.f);
	P.w_float_q8(q.z,-1.f,1.f);
	P.w_float_q8(q.w,-1.f,1.f);
}
static void w_qt_q8(CSaveChunk* P, const Fquaternion& q)
{
	P->w_float(q.x);
	P->w_float(q.y);
	P->w_float(q.z);
	P->w_float(q.w);
}

template<typename src>
static void r_qt_q8(src& P,Fquaternion& q)
{
	q.x=P.r_float_q8(-1.f,1.f);
	q.y=P.r_float_q8(-1.f,1.f);
	q.z=P.r_float_q8(-1.f,1.f);
	q.w=P.r_float_q8(-1.f,1.f);

	clamp(q.x,-1.f,1.f);
	clamp(q.y,-1.f,1.f);
	clamp(q.z,-1.f,1.f);
	clamp(q.w,-1.f,1.f);
}

static void r_qt_q8(CSaveChunk* P, Fquaternion& q)
{
	P->r_float(q.x);
	P->r_float(q.y);
	P->r_float(q.z);
	P->r_float(q.w);

	clamp(q.x, -1.f, 1.f);
	clamp(q.y, -1.f, 1.f);
	clamp(q.z, -1.f, 1.f);
	clamp(q.w, -1.f, 1.f);
}
static void s_qt_q8(ISaveObject& P, Fquaternion& q)
{
	if (P.IsSave()) 
	{
		clamp(q.x, -1.f, 1.f);
		clamp(q.y, -1.f, 1.f);
		clamp(q.z, -1.f, 1.f);
		clamp(q.w, -1.f, 1.f);
	}
	P << q;
	if (!P.IsSave())
	{
		clamp(q.x, -1.f, 1.f);
		clamp(q.y, -1.f, 1.f);
		clamp(q.z, -1.f, 1.f);
		clamp(q.w, -1.f, 1.f);
	}
}

#ifdef XRGAME_EXPORTS
/////////////////////////////////16////////////////////////////////////////////////////////////////
static void w_vec_q16(NET_Packet& P,const Fvector& vec,const Fvector& min,const Fvector& max)
{
	P.w_float_q16(vec.x,min.x,max.x);
	P.w_float_q16(vec.y,min.y,max.y);
	P.w_float_q16(vec.z,min.z,max.z);
}
static void r_vec_q16(NET_Packet& P,Fvector& vec,const Fvector& min,const Fvector& max)
{
	P.r_float_q16(vec.x,min.x,max.x);
	P.r_float_q16(vec.y,min.y,max.y);
	P.r_float_q16(vec.z,min.z,max.z);
	
	//clamp(vec.x,min.x,max.x);
	//clamp(vec.y,min.y,max.y);
	//clamp(vec.z,min.z,max.z);
}
template<typename src>
static void w_qt_q16(src& P,const Fquaternion& q)
{
	//Fvector Q;
	//Q.set(q.x,q.y,q.z);
	//if(q.w<0.f)	Q.invert();
	//P.w_float_q16(Q.x,-1.f,1.f);
	//P.w_float_q16(Q.y,-1.f,1.f);
	//P.w_float_q16(Q.z,-1.f,1.f);
	///////////////////////////////////////////
	P.w_float_q16(q.x,-1.f,1.f);
	P.w_float_q16(q.y,-1.f,1.f);
	P.w_float_q16(q.z,-1.f,1.f);
	P.w_float_q16(q.w,-1.f,1.f);
}

static void r_qt_q16(NET_Packet& P,Fquaternion& q)
{
	// x^2 + y^2 + z^2 + w^2 = 1
	//P.r_float_q16(q.x,-1.f,1.f);
	//P.r_float_q16(q.y,-1.f,1.f);
	//P.r_float_q16(q.z,-1.f,1.f);
	//float w2=1.f-q.x*q.x-q.y*q.y-q.z*q.z;
	//w2=w2<0.f ? 0.f : w2;
	//q.w=_sqrt(w2);
///////////////////////////////////////////////////
	P.r_float_q16(q.x,-1.f,1.f);
	P.r_float_q16(q.y,-1.f,1.f);
	P.r_float_q16(q.z,-1.f,1.f);
	P.r_float_q16(q.w,-1.f,1.f);

	//clamp(q.x,-1.f,1.f);
	//clamp(q.y,-1.f,1.f);
	//clamp(q.z,-1.f,1.f);
	//clamp(q.w,-1.f,1.f);
}
#endif
///////////////////////////////////////////////////////////////////////////////////
void	SPHNetState::net_Export(NET_Packet& P)
{
	P.w_vec3(linear_vel);
	//P.w_vec3(angular_vel);
	//P.w_vec3(force);
	//P.w_vec3(torque);
	P.w_vec3(position);
	P.w_vec4(*((Fvector4*)&quaternion));
	//P.w_vec4(*((Fvector4*)&previous_quaternion));
	P.w_u8((u8)enabled);

}

template<typename src>
void	SPHNetState::read				(src&			P)
{
	linear_vel=P.r_vec3();
	angular_vel.set(0.f,0.f,0.f);		//P.r_vec3(angular_vel);
	force.set(0.f,0.f,0.f);				//P.r_vec3(force);
	torque.set(0.f,0.f,0.f);			//P.r_vec3(torque);
	position=P.r_vec3();
	*((Fvector4*)&quaternion)=P.r_vec4();
	previous_quaternion.set(quaternion);//P.r_vec4(*((Fvector4*)&previous_quaternion));
	enabled=!!P.r_u8	();
}
template<typename src>
void	SPHNetState::read(src* P)
{
	P->r_vec3(linear_vel);
	angular_vel.set(0.f, 0.f, 0.f);		//P.r_vec3(angular_vel);
	force.set(0.f, 0.f, 0.f);				//P.r_vec3(force);
	torque.set(0.f, 0.f, 0.f);			//P.r_vec3(torque);
	P->r_vec3(position);
	Fvector4* q = ((Fvector4*)&quaternion);
	P->r_vec4(*q);
	previous_quaternion.set(quaternion);//P.r_vec4(*((Fvector4*)&previous_quaternion));
	P->r_bool(enabled);
}
template<>
void	SPHNetState::read(ISaveObject& P)
{
	P << linear_vel;
	angular_vel.set(0.f, 0.f, 0.f);		//P.r_vec3(angular_vel);
	force.set(0.f, 0.f, 0.f);				//P.r_vec3(force);
	torque.set(0.f, 0.f, 0.f);			//P.r_vec3(torque);
	P << position << quaternion;
	//Fvector4* q = ((Fvector4*)&quaternion);
	//P->r_vec4(*q);
	previous_quaternion.set(quaternion);//P.r_vec4(*((Fvector4*)&previous_quaternion));
	P << enabled;
}

void	SPHNetState::net_Import(NET_Packet&	P)
{
	read(P);
}
void SPHNetState::net_Import(IReader& P)
{
	read(P);
}

void SPHNetState::net_Save(NET_Packet &P)
{
	net_Export(P);
}

void SPHNetState::net_Load(NET_Packet &P)
{
	net_Import(P);
	previous_position.set(position);
}

void SPHNetState::net_Load(IReader &P)
{
	net_Import(P);
	previous_position.set(position);
}

void SPHNetState::net_Save(NET_Packet &P,const Fvector& min,const Fvector& max)
{
	w_vec_q8(P, position, min, max);
	w_qt_q8(P, quaternion);
	P.w_u8((u8)enabled);
}

template<typename src>
void SPHNetState::read(src &P,const Fvector& min,const Fvector& max)
{
	VERIFY(!(fsimilar(min.x, max.x) && fsimilar(min.y, max.y) && fsimilar(min.z, max.z)));
	linear_vel.set(0.f, 0.f, 0.f);
	angular_vel.set(0.f, 0.f, 0.f);
	force.set(0.f, 0.f, 0.f);
	torque.set(0.f, 0.f, 0.f);
	r_vec_q8(P, position, min, max);
	previous_position.set(position);
	r_qt_q8(P, quaternion);
	previous_quaternion.set(quaternion);
	enabled = !!P.r_u8();

}
template<typename src>
void SPHNetState::read(src* P, const Fvector& min, const Fvector& max)
{
	VERIFY(!(fsimilar(min.x, max.x) && fsimilar(min.y, max.y) && fsimilar(min.z, max.z)));
	linear_vel.set(0.f, 0.f, 0.f);
	angular_vel.set(0.f, 0.f, 0.f);
	force.set(0.f, 0.f, 0.f);
	torque.set(0.f, 0.f, 0.f);
	r_vec_q8(P, position, min, max);
	previous_position.set(position);
	r_qt_q8(P, quaternion);
	previous_quaternion.set(quaternion);
	enabled = !!P->r_u8();

}
/*template<>
void SPHNetState::read(CSaveChunk* P, const Fvector& min, const Fvector& max)
{
	VERIFY(!(fsimilar(min.x, max.x) && fsimilar(min.y, max.y) && fsimilar(min.z, max.z)));
	linear_vel.set(0.f, 0.f, 0.f);
	angular_vel.set(0.f, 0.f, 0.f);
	force.set(0.f, 0.f, 0.f);
	torque.set(0.f, 0.f, 0.f);
	r_vec_q8(P, position, min, max);
	previous_position.set(position);
	r_qt_q8(P, quaternion);
	previous_quaternion.set(quaternion);
	P->r_bool(enabled);

}*/

void SPHNetState::net_Load(NET_Packet &P,const Fvector& min,const Fvector& max)
{
	VERIFY(!(fsimilar(min.x, max.x) && fsimilar(min.y, max.y) && fsimilar(min.z, max.z)));
	read(P, min, max);
}

void SPHNetState::net_Load(IReader &P,const Fvector& min,const Fvector& max)
{
	VERIFY(!(fsimilar(min.x, max.x) && fsimilar(min.y, max.y) && fsimilar(min.z, max.z)));
	read(P, min, max);
}

void SPHNetState::net_Serialize(ISaveObject& Object)
{
	Object.BeginChunk("SPHNetState");
	{
		Object << linear_vel << position << quaternion << enabled;
	}
	Object.EndChunk();
}

void SPHNetState::net_Serialize(ISaveObject& Object, const Fvector& min, const Fvector& max)
{
	Object.BeginChunk("SPHNetState");
	{
		s_vec_q8(Object, position, min, max);
		s_qt_q8(Object, quaternion);
		Object << enabled;
	}
	Object.EndChunk();
}

/*CSaveObject& SPHNetState::operator<<(CSaveObject& Object)
{
	net_Serialize(Object);
	return Object;
}*/

/*void SPHNetState::net_Save(CSaveObjectSave* Object) const
{
	Object->BeginChunk("SPHNetState");
	{
		Object->GetCurrentChunk()->w_vec3(linear_vel);
		Object->GetCurrentChunk()->w_vec3(position);
		Object->GetCurrentChunk()->w_vec4(*((Fvector4*)&quaternion));
		Object->GetCurrentChunk()->w_bool(enabled);
	}
	Object->EndChunk();
}

void SPHNetState::net_Load(CSaveObjectLoad* Object)
{
	Object->BeginChunk("SPHNetState");
	{
		read(Object->GetCurrentChunk());
		previous_position.set(position);
	}
	Object->EndChunk();
}

void SPHNetState::net_Save(CSaveObjectSave* Object, const Fvector& min, const Fvector& max) const
{
	Object->BeginChunk("SPHNetState");
	{
		w_vec_q8(Object->GetCurrentChunk(), position, min, max);
		w_qt_q8(Object->GetCurrentChunk(), quaternion);
		Object->GetCurrentChunk()->w_bool(enabled);
	}
	Object->EndChunk();
}

void SPHNetState::net_Load(CSaveObjectLoad* Object, const Fvector& min, const Fvector& max)
{
	Object->BeginChunk("SPHNetState");
	{
		VERIFY(!(fsimilar(min.x, max.x) && fsimilar(min.y, max.y) && fsimilar(min.z, max.z)));
		read(Object->GetCurrentChunk(), min, max);
	}
	Object->EndChunk();
}*/

SPHBonesData::SPHBonesData()
{
	bones_mask.set_all();
	root_bone = 0;

	Fvector _mn, _mx;

	_mn.set(-100.f, -100.f, -100.f);
	_mx.set(100.f, 100.f, 100.f);
	set_min_max(_mn, _mx);
}

void SPHBonesData::net_Save(NET_Packet &P)
{
	for (u64 i = 0; i < VisMask::ArrSize; ++i) {
		P.w_u64(bones_mask.GetRawChunkData(i));
	}
	//P.w_u64(bones_mask._visimask.flags);
	P.w_u16(root_bone);

	P.w_vec3(get_min());
	P.w_vec3(get_max());
	P.w_u16((u16)bones.size());//bones number;

	//if(bones.size() > 64) {
	//	Msg("!![SPHBonesData::net_Save] bones_size is [%u]!", bones.size());
	//	P.w_u64(bones_mask._visimask_ex.flags);
	//}

	PHNETSTATE_I i = bones.begin(), e = bones.end();
	for(; e != i; i++) {
		(*i).net_Save(P, get_min(), get_max());
	}
}

void SPHBonesData::net_Load(NET_Packet &P)
{
	bones.clear();

	// VisMask init 
	//u64 _low = P.r_u64(); // Left (0...64)
	//u64 _high = u64(-1); // Right(64..128)
	for (u64 i = 0; i < VisMask::ArrSize; ++i) {
		P.w_u64(bones_mask.GetRawChunkData(i));
		bones_mask.set(P.r_u64(), true, i);
	}

	root_bone = P.r_u16();
	Fvector _mn, _mx;
	P.r_vec3(_mn);
	P.r_vec3(_mx);
	set_min_max(_mn, _mx);

	u16 bones_number = P.r_u16();
	//if(bones_number > 64) {
	//	Msg("!![SPHBonesData::net_Load] bones_number is [%u]!", bones_number);
	//	_high = P.r_u64();
	//}
	//bones_mask.set(_low, _high);

	for(int i = 0; i < bones_number; i++) {
		SPHNetState	S;
		S.net_Load(P, get_min(), get_max());
		bones.push_back(S);
	}
}

/*void SPHBonesData::net_Save(CSaveObjectSave* Object) const
{
	Object->BeginChunk("SPHBonesData");
	{
		for (u64 i = 0; i < VisMask::ArrSize; ++i) {
			P.w_u64(bones_mask.GetRawChunkData(i));
		}

		Object->GetCurrentChunk()->w_u64(bones_mask._visimask.flags);
		Object->GetCurrentChunk()->w_u16(root_bone);

		Object->GetCurrentChunk()->w_vec3(get_min());
		Object->GetCurrentChunk()->w_vec3(get_max());
		Object->GetCurrentChunk()->w_u16((u16)bones.size());//bones number;

		if (bones.size() > 64) {
			Msg("!![SPHBonesData::net_Save] bones_size is [%u]!", bones.size());
			Object->GetCurrentChunk()->w_u64(bones_mask._visimask_ex.flags);
		}

		Object->GetCurrentChunk()->WriteArray(bones.size());
		{
			for (const auto& bone : bones) {
				bone.net_Save(Object, get_min(), get_max());
			}
		}
	}
	Object->EndChunk();
}

void SPHBonesData::net_Load(CSaveObjectLoad* Object)
{
	Object->BeginChunk("SPHBonesData");
	{
		bones.clear();

		// VisMask init 
		u64 _low; // Left (0...64)
		u64 _high = u64(-1); // Right(64..128)
		Fvector _mn, _mx;

		for (u64 i = 0; i < VisMask::ArrSize; ++i) {
			P.w_u64(bones_mask.GetRawChunkData(i));
			bones_mask.set(P.r_u64(), true, i);
		}

		Object->GetCurrentChunk()->r_u64(_low);
		Object->GetCurrentChunk()->r_u16(root_bone);
		Object->GetCurrentChunk()->r_vec3(_mn);
		Object->GetCurrentChunk()->r_vec3(_mx);
		set_min_max(_mn, _mx);

		u16 bones_number;
		Object->GetCurrentChunk()->r_u16(bones_number);
		if (bones_number > 64) {
			Msg("!![SPHBonesData::net_Load] bones_number is [%u]!", bones_number);
			Object->GetCurrentChunk()->r_u64(_high);
		}
		bones_mask.set(_low, _high);

		for (int i = 0; i < bones_number; i++) {
			SPHNetState	S;
			S.net_Load(Object, get_min(), get_max());
			bones.push_back(S);
		}
	}
	Object->EndChunk();
}*/

void SPHBonesData::net_Serialize(ISaveObject& Object)
{
	//auto PerElemAction = [&](SPHNetState& Elem) {Elem.net_Serialize(Object, get_min(), get_max()); };
	Object.BeginChunk("SPHBonesData");
	{
		bones.clear();
		Object << bones_mask << root_bone;
		if (Object.IsSave()) {
			Fvector _mn = get_min(), _mx = get_max();
			Object << _mn << _mx;/*<void, SPHBonesData, SPHBonesData, CSaveObject&, SPHNetState&>*/ 
			((CSaveObject&)Object).Serialize(bones, fastdelegate::MakeDelegate(this, &SPHBonesData::PerElemAction));

			/*Object->GetCurrentChunk()->WriteArray(bones.size());
			{
				for (const auto& bone : bones) {
					bone.net_Serialize(Object, get_min(), get_max());
				}
			}*/
		}
		else {
			Fvector _mn, _mx;
			u16 bones_number;
			Object << _mn << _mx << bones_number;
			set_min_max(_mn, _mx);
			((CSaveObject&)Object).Serialize(bones, fastdelegate::MakeDelegate(this, &SPHBonesData::PerElemAction));

			/*for (int i = 0; i < bones_number; i++) {
				SPHNetState	S;
				S.net_Serialize(Object, get_min(), get_max());
				bones.push_back(S);
			}*/
		}

		//u16 bones_number;
		//Object->GetCurrentChunk()->r_u16(bones_number);
		//if (bones_number > 64) {
		//	Msg("!![SPHBonesData::net_Load] bones_number is [%u]!", bones_number);
		//	Object->GetCurrentChunk()->r_u64(_high);
		//}
		//bones_mask.set(_low, _high);

		/*for (int i = 0; i < bones_number; i++) {
			SPHNetState	S;
			S.net_Load(Object, get_min(), get_max());
			bones.push_back(S);
		}*/
	}
	Object.EndChunk();
}

void SPHBonesData::set_min_max(const Fvector& _min, const Fvector& _max)
{
	VERIFY( !_min.similar(_max) );
	m_min = _min;
	m_max = _max;
}
