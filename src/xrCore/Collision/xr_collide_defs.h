#pragma once

#include "xrCDB.h"

class CObject;
namespace collide 
{
	struct			tri {
		Fvector	e10; float e10s;
		Fvector	e21; float e21s;
		Fvector	e02; float e02s;
		Fvector p[3];
		Fvector N;
		float	d;
	};
	struct			elipsoid {
		Fmatrix	mL2W;		// convertion from sphere(000,1) to real space
		Fmatrix	mW2L;		// convertion from real space to sphere(000,1)
	};
	struct			ray_cache
	{
		// previous state
		Fvector				start;
		Fvector				dir;
		float				range;
		BOOL				result;

		// cached vertices
		Fvector				verts[3];
		ray_cache() 
		{
			start.set	(0,0,0);
			dir.set		(0,0,0);
			range		= 0;
			result		= FALSE;
			verts[0].set(0,0,0);
			verts[1].set(0,0,0);
			verts[2].set(0,0,0);
		}
		void				set		(const Fvector& _start, const Fvector& _dir, const float _range,const BOOL _result)
		{
			start	= _start;
			dir		= _dir;
			range	= _range;
			result	= _result;
		}
		BOOL				similar	(const Fvector& _start, const Fvector& _dir, const float _range)
		{
			if (!_start.similar(start))					return FALSE;
			if (!fsimilar(1.f,dir.dotproduct(_dir)))	return FALSE;
			if (!fsimilar(_range,range))				return FALSE;
			return			TRUE;
		}
	};
	enum rq_target	{
		rqtNone		= (0),
		rqtObject	= (1<<0),
		rqtStatic	= (1<<1),
		rqtShape	= (1<<2),
		rqtObstacle	= (1<<3),
		rqtBoth		= (rqtObject|rqtStatic),
		rqtDyn		= (rqtObject|rqtShape|rqtObstacle)
	};
	struct			ray_defs
	{
		Fvector		start;
		Fvector		dir;
		float		range;
		u32			flags;
		rq_target	tgt;
		ray_defs	(const Fvector& _start, const Fvector& _dir, float _range, u32 _flags, rq_target _tgt)
		{
			start	= _start;
			dir		= _dir;
			range	= _range;
			flags	= _flags;
			tgt		= _tgt;
		}
		ray_defs	(const Fvector& _start, const Fvector& _end, u32 _flags, rq_target _tgt)
		{
			start	= _start;
			Fvector temp = _end;
			dir		= temp.sub(_start).normalize();
			range	= _start.distance_to(_end);
			flags	= _flags;
			tgt		= _tgt;
		}
	};
	struct			rq_result 
	{
		struct DynamicData
		{
			CObject* object;
			int bone_id;
		};
		struct StaticData
		{
			Fvector poly[3];
			CDB::TRIExtra tris;
		};
		bool IsDynamic = false;
		float range = std::numeric_limits<float>::infinity();
		union
		{
			DynamicData d;
			StaticData s;
		} data{};

		IC CObject* object() const { R_ASSERT(IsDynamic); return data.d.object; }
		IC int bone_id() const { R_ASSERT(IsDynamic); return data.d.bone_id; }
		IC const CDB::TRIExtra& tri() const { R_ASSERT(!IsDynamic); return data.s.tris; }
		IC const Fvector (&poly())[3] { R_ASSERT(!IsDynamic); return data.s.poly; }

		IC void CopyPoly(Fvector (&other)[3])
		{
			R_ASSERT(!IsDynamic);
			std::memcpy(&other, &data.s.poly, sizeof(data.s.poly));
		}
		
		//CObject*	O;				// if NULL - static
		//float		range;			// range to intersection
		//int			element;		// номер кости/номер треугольника
		IC rq_result& set(CObject& obj, float range, int bone_id)
		{			
			IsDynamic = true;
			this->range = range;
			data.d.object = &obj;
			data.d.bone_id = bone_id;
			return *this;
		}
		IC rq_result& set(const CDB::TRIExtra& tri, const Fvector (&poly)[3], float range)
		{
			IsDynamic = false;
			this->range = range;
			data.s.tris = tri;
			memcpy(data.s.poly, poly, sizeof(data.s.poly));
			return *this;
		}
		IC void Copy(const rq_result& src)
		{
			IsDynamic = src.IsDynamic;
			range = src.range;
			std::memcpy(&data, &src.data, sizeof(data));
		}
		/*IC rq_result& set		(CObject* _O, float _range, int _element)
		{
			O		= _O;
			range	= _range;
			element	= _element;
			return	*this;
		}*/
		/*IC bool set_if_less(CDB::RESULT& other)
		{
			if (other.range < range)
			{
				IsDynamic = false;
				range = other.range;
				data.s.tris.data = other.data;
				data.s.tris.verts = other.
			}
		}*/
		IC bool set_if_less(const rq_result& other)
		{
			if (other.range < range)
			{
				Copy(other);
				return true;
			}
			return false;
		}
		IC bool set_if_less(const CDB::TRIExtra& tri, const Fvector (&poly)[3], float range)
		{
			if (this->range > range)
			{
				IsDynamic = false;
				this->range = range;
				data.s.tris = tri;
				std::memcpy(&data.s.poly, &poly, sizeof(data.s.poly));
				return true;
			}
			return false;
		}
		IC bool set_if_less(CObject& obj, float range, int bone_id)
		{
			if (this->range > range)
			{
				IsDynamic = true;
				this->range = range;
				data.d.object = &obj;
				data.d.bone_id = bone_id;
				return true;
			}
			return false;
		}
		
		//IC BOOL		set_if_less	(CDB::RESULT*	I){if (I->range<range){ set(0,I->range,I->id);			return TRUE;}else return FALSE;}
		//IC BOOL		set_if_less	(rq_result*		R){if (R->range<range){ set(R->O,R->range,R->element);	return TRUE;}else return FALSE;}
		//IC BOOL		set_if_less	(CObject* _who, float _range, int _element)	{ if (_range<range) { set(_who,_range,_element); return TRUE;}else return FALSE;}
		/*IC BOOL		valid		()
		{
			return IsDynamic ? data.d.object ? true;
		}*/
	};

	using rqVec = xr_vector<rq_result>;
	using rqIt = rqVec::iterator;

	class			rq_results
	{
	protected:
		rqVec		results;
		static bool	r_sort_pred		(const rq_result& a, const rq_result& b)	{	return a.range<b.range;}
	public:
		IC bool append_result(const CDB::TRIExtra& tri, const Fvector (&poly)[3], float range, bool bNearest)
		{
			if (bNearest&&!results.empty()){
				rq_result& R		= results.back();
				if (range<R.range){
					R.set(tri, poly, range);
					return true;
				}
				return false;
			}
			rq_result& rq = results.emplace_back();
			rq.set(tri, poly, range);
			return true;
		}
		IC bool append_result(CObject& _who, float _range, int _element, bool bNearest)
		{
			if (bNearest&&!results.empty()){
				rq_result& R		= results.back();
				if (_range<R.range){
					R.set(_who, _range, _element);
					return true;
				}
				return false;
			}
			rq_result& rq = results.emplace_back();
			rq.set(_who, _range, _element);
			return true;
		}
		IC void		append_result	(const rq_result& res)
		{
			if (0==results.capacity())
			{
				results.reserve(8);
			}
			results.push_back			(res);
		}
		IC int			r_count			()	{ return (int)results.size();	}
		IC rq_result*	r_begin			()	{ return &*results.begin();	}
		IC rq_result*	r_end			()	{ return &*results.end();	}
		IC void			r_clear			()	{ results.resize(0);	}
		IC void			r_sort			()	{ std::sort(results.begin(),results.end(),r_sort_pred);}
		IC rqVec		&r_results		()	{ return results; }

	};
	typedef  BOOL		rq_callback 	(rq_result& result, LPVOID user_data);
	typedef  BOOL		test_callback 	(const ray_defs& rd, CObject* object, LPVOID user_data);
};