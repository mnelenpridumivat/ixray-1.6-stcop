#ifndef _INCDEF_NETUTILS_H_
#define _INCDEF_NETUTILS_H_
#pragma once

#include "client_id.h"
#include "MemoryBuffer.h"

#pragma pack(push,1)


struct XRCORE_API IIniFileStream
{
	virtual void	 __stdcall 	move_begin		()							= 0;

	virtual void	 __stdcall 	w_float			( float a)					= 0;
	virtual void 	 __stdcall 	w_vec3			( const Fvector& a)			= 0;
	virtual void 	 __stdcall 	w_vec4			( const Fvector4& a)		= 0;
	virtual void 	 __stdcall 	w_u64			( u64 a)					= 0;
	virtual void 	 __stdcall 	w_s64			( s64 a)					= 0;
	virtual void 	 __stdcall 	w_u32			( u32 a)					= 0;
	virtual void 	 __stdcall 	w_s32			( s32 a)					= 0;
	virtual void 	 __stdcall 	w_u16			( u16 a)					= 0;
	virtual void 	__stdcall	w_s16			( s16 a)					= 0;
	virtual void	__stdcall	w_u8			( u8 a)						= 0;
	virtual void	__stdcall	w_s8			( s8 a)						= 0;
	virtual void	__stdcall	w_stringZ		( LPCSTR S)					= 0;
					
	virtual void	__stdcall	r_vec3			(Fvector&)					= 0;
	virtual void	__stdcall	r_vec4			(Fvector4&)					= 0;
	virtual void	__stdcall	r_float			(float&)					= 0;
	virtual void	__stdcall	r_u8			(u8&)						= 0;
	virtual void	__stdcall	r_u16			(u16&)						= 0;
	virtual void	__stdcall	r_u32			(u32&)						= 0;
	virtual void	__stdcall	r_u64			(u64&)						= 0;
	virtual void	__stdcall	r_s8			(s8&)						= 0;
	virtual void	__stdcall	r_s16			(s16&)						= 0;
	virtual void	__stdcall	r_s32			(s32&)						= 0;
	virtual void	__stdcall	r_s64			(s64&)						= 0;

	virtual void	__stdcall	r_string		(LPSTR dest, u32 dest_size)	= 0;
//	virtual void	__stdcall	r_tell			()							= 0;
//	virtual void	__stdcall	r_seek			(u32 pos)					= 0;
	virtual void	__stdcall	skip_stringZ	()							= 0;
};


#define INI_W(what_to_do)\
if(inistream)\
{\
	inistream->what_to_do;\
}

#ifdef SHIPPING_BUILD
#define INI_ASSERT(what_to_do)
#else
#define INI_ASSERT(what_to_do)\
{\
if(inistream)\
	R_ASSERT3(0,#what_to_do,"not implemented");\
}
#endif

struct	NET_Buffer
{
	BYTE	data	[NET_PacketSizeLimit];
	u32		count;
};

class XRCORE_API NET_Packet_Wrapper;

class XRCORE_API NET_Packet
{
	friend struct NET_Packet_Wrapper;
public:
	IIniFileStream* inistream;

    //void            construct( const void* data, unsigned size )
    //                {
    //                    memcpy( B.data, data, size );
    //                    B.count = size;
    //                }
                    
	//NET_Buffer		B;
	xr_shared_ptr<MemoryBuffer::IBufferBase> Buffer = nullptr;
	//u32				r_pos;
	u32				timeReceive;
	bool			w_allow;
private:
	NET_Packet			():inistream(NULL),w_allow(true)	{}
public:
	// writing - main
	void write_start	();
	void w_begin		( u16 type	);

	struct W_guard{
		bool*	guarded;
		W_guard(bool* b):guarded(b){*b=true;}
		~W_guard(){*guarded=false;}
	};
	void w( const void* p, u32 count );

	// read/write operators
	template <typename T>
	void operator<<(const T& value)
	{
		w(&value, (u32)sizeof(T));
	}

	template <typename T>
	void operator>>(T& value)
	{
		r(&value, (u32)sizeof(T));
	}

	void w_seek	(u32 pos, const void* p, u32 count);
	u32	w_tell	();

	// writing - utilities
	void w_float		( float a       );			// float
	void w_vec3		( const Fvector& a);			// vec3
	void w_vec4		( const Fvector4& a);			// vec4
	void w_u64		( u64 a			);			// qword (8b)
	void w_s64		( s64 a			);			// qword (8b)
	void w_u32		( u32 a			);			// dword (4b)
	void w_s32		( s32 a			);			// dword (4b)
	void w_u16		( u16 a			);			// word (2b)
	void w_s16		( s16 a			);			// word (2b)
	void w_u8		( u8 a			);			// byte (1b)
	void w_s8		( s8 a			);			// byte (1b)

	void w_float_q16	( float a, float min, float max);
	void w_float_q8	( float a, float min, float max);
	void w_angle16	( float a		);
	void w_angle8	( float a		);
	void w_dir		( const Fvector& D);
	void w_sdir		( const Fvector& D);
	void w_stringZ			( LPCSTR S );
	void w_stringZ			( const shared_str& p);
	void w_matrix			(Fmatrix& M);
	
	void w_clientID			(ClientID& C);
	
	void w_chunk_open8		(u32& position);
	
	void w_chunk_close8		(u32 position);

	void w_chunk_open16		(u32& position);

	void w_chunk_close16		(u32 position);

	// reading
	void		read_start		();
	u32			r_begin			( u16& type	);
	void		r_seek			(u32 pos);
	u32			r_tell			();

	void		r				( void* p, u32 count);
	BOOL		r_eof			();
	u32			r_elapsed		();
	void		r_advance		(u32 size);

	// reading - utilities
	void		r_vec3			(Fvector& A);
	void		r_vec4			(Fvector4& A);
	void		r_float			(float& A );
	void 		r_u64			(u64& A);
	void 		r_s64			(s64& A);
	void 		r_u32			(u32& A);
	void		r_s32			(s32& A);
	void		r_u16			(u16& A);
	void		r_s16			(s16& A);
	void		r_u8			(u8&  A);
	void		r_s8			(s8&  A);

	// IReader compatibility
	Fvector		r_vec3			();
	Fvector4	r_vec4			();
	float		r_float_q8		(float min,float max);
	float		r_float_q16		(float min, float max);
	float		r_float			();
	u64 		r_u64			();
	s64 		r_s64			();
	u32 		r_u32			();
	s32			r_s32			();
	u16			r_u16			();
	s16			r_s16			();
	u8			r_u8			();
	s8			r_s8			();

	void		r_float_q16		(float& A, float min, float max);
	void		r_float_q8		(float& A, float min, float max);
	void		r_angle16		(float& A);
	void		r_angle8		(float& A);
	void		r_dir			(Fvector& A);

	void		r_sdir			(Fvector& A);
	void		r_stringZ		(LPSTR S );
	void		r_stringZ		(xr_string& dest );
	void 		r_stringZ		(shared_str& dest);
	
	void		skip_stringZ	();
	
	void		r_stringZ_s		(LPSTR string, u32 size);

	template <u32 size>
	inline void	r_stringZ_s		(char (&string)[size])
	{
		r_stringZ_s	(string, size);
	}

	void		r_matrix		(Fmatrix& M);
	void		r_clientID		(ClientID& C);
};

struct XRCORE_API NET_Packet_Wrapper {
	static NET_Packet CreateNetwork();
	static NET_Packet* CreateNetworkPtr();
	static NET_Packet CreateSerialize();
};

#pragma pack(pop)

#endif /*_INCDEF_NETUTILS_H_*/