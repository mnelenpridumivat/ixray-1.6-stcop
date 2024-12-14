#pragma once
#include "Save/SaveInterface.h"
#pragma pack(push,4)
//////////////////////////////////////////////////////////////////////////
using str_c = const char*;

#pragma warning(disable : 4200)
struct XRCORE_API str_value
{
	u32 dwReference;
	u32 dwLength;
	u32 dwCRC;

	str_value* next;
	char value[];
};

struct XRCORE_API str_value_cmp 
{
	IC bool operator ()	(const str_value* A, const str_value* B) const { return A->dwCRC < B->dwCRC; };
};

struct XRCORE_API str_hash_function 
{
	IC size_t operator ()(str_value const* const value) const { return value->dwCRC; };
};

#pragma warning(default : 4200)

struct str_container_impl;
class IWriter;
//////////////////////////////////////////////////////////////////////////
class XRCORE_API str_container
{
private:
	xrCriticalSection cs;
	str_container_impl* impl;

	str_container();

public:
	str_container(const str_container& other) = delete;
	str_container(str_container&& other) = delete;
	str_container& operator=(const str_container& other) = delete;
	str_container& operator=(str_container&& other) = delete;

	static str_container& GetInstance();

	~str_container();

	str_value* dock(str_c value);
	void clean();
	void dump();
	void dump(IWriter* W);
	void verify();
	u32  stat_economy();
//#ifdef PROFILE_CRITICAL_SECTIONS
//	str_container() :cs(MUTEX_PROFILE_ID(str_container)) {}
//#endif // PROFILE_CRITICAL_SECTIONS
};

/*class XRCORE_API str_container_handle {

	str_container* pStringContainer = nullptr;

	str_container_handle(){}

public:
	~str_container_handle();

	str_container_handle(const str_container_handle& other) = delete;
	str_container_handle(str_container_handle&& other) = delete;
	str_container_handle& operator=(const str_container_handle& other) = delete;
	str_container_handle& operator=(str_container_handle&& other) = delete;

	static str_container_handle& GetInstance();

	str_container* get_container();

};*/

//#define g_pStringContainer str_container_handle::GetInstance().get_container()
//XRCORE_API extern str_container* g_pStringContainer;

//////////////////////////////////////////////////////////////////////////
class XRCORE_API shared_str
{
	str_value* p_;
protected:
	void _dec() { if (0 == p_) return;	p_->dwReference--; 	if (0 == p_->dwReference)	p_ = 0; }
public:
	void _set(str_c rhs) { str_value* v = str_container::GetInstance().dock(rhs); if (0 != v) v->dwReference++; _dec(); p_ = v; }
	void _set(shared_str const& rhs) { str_value* v = rhs.p_; if (0 != v) v->dwReference++; _dec(); p_ = v; }

	const str_value* _get()	const { return p_; }

	inline char operator[](size_t index) const;
public:
	// construction
	shared_str() { p_ = 0; }
	shared_str(str_c rhs) { p_ = 0;	_set(rhs); }
	shared_str(shared_str const& rhs) { p_ = 0;	_set(rhs); }
	~shared_str() { _dec(); }

	// assignment & accessors
	shared_str&			operator=	(str_c rhs) { _set(rhs);	return (shared_str&)*this; }
	shared_str&			operator=	(shared_str const& rhs) { _set(rhs);	return (shared_str&)*this; }
	str_c				operator*	() const { return p_ ? p_->value : 0; }
	bool				operator!	() const { return p_ == 0; }
	char				operator[]	(size_t id) { return p_->value[id]; }
	str_c				c_str		() const { return p_ ? p_->value : 0; }

	// misc func
	u32					size		() const { if (0 == p_) return 0; else return p_->dwLength; }
	void				swap		(shared_str& rhs) { str_value* tmp = p_; p_ = rhs.p_; rhs.p_ = tmp; }
	bool				equal		(const shared_str& rhs) const { return (p_ == rhs.p_); }
	shared_str& 		printf		(const char* format, ...);
};


// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr
IC bool operator	==	(shared_str const& a, shared_str const& b) { return a._get() == b._get(); }
IC bool operator	!=	(shared_str const& a, shared_str const& b) { return a._get() != b._get(); }
IC bool operator	<	(shared_str const& a, shared_str const& b) { return a._get() < b._get();  }
IC bool operator	>	(shared_str const& a, shared_str const& b) { return a._get() > b._get();  }

// externally visible standart functionality
IC void swap		(shared_str& lhs, shared_str& rhs)		{ lhs.swap(rhs); }
IC u32	xr_strlen	(shared_str& a)							{ return a.size(); }
IC int	xr_strcmp	(const shared_str& a, const char* b)	{ return xr_strcmp(*a, b); }
IC int	xr_strcmp	(const char* a, const shared_str& b)	{ return xr_strcmp(a, *b); }
IC int	xr_strcmp	(const shared_str& a, const shared_str& b) 
{
	if (a.equal(b))		return 0;
	else				return xr_strcmp(*a, *b);
}

IC void	xr_strlwr(shared_str& src) { if (*src) { LPSTR lp = xr_strdup(*src); xr_strlwr(lp); src = lp; xr_free(lp); } }

namespace std
{
	template<>
	class hash<shared_str>
	{
	public:
		using is_transparent = void;
	public:
		size_t operator()(const shared_str& s) const
		{
			return s._get()->dwCRC;
		}
	};
}

ISaveObject& operator<<(ISaveObject& Object, shared_str& Value);

#pragma pack(pop)