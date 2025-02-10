#include "stdafx.h"
#pragma hdrstop
#include "net_utils.h"

// ---NET_Packet
// reading
void NET_Packet::read_start()	{	
	Buffer->r_start();
	//r_pos		= 0; 
	INI_W(move_begin());
}

u32 NET_Packet::r_begin			( u16& type	)	// returns time of receiving
{
	Buffer->r_start();
	//r_pos		= 0;
	if(!inistream)
		r_u16		(type);
	else
		inistream->r_u16(type);

	return		timeReceive;
}


// writing - main

void NET_Packet::write_start() { 
	//B.count = 0;	
	Buffer->w_start();
	INI_W(move_begin()); 
}

void NET_Packet::w_begin(u16 type) { 
	//B.count = 0;
	Buffer->w_start();
	w_u16(type); 
}

void NET_Packet::w(const void* p, u32 count)
{
	R_ASSERT(inistream == NULL || w_allow);
	VERIFY(p && count);
	Buffer->w(p, count, MemoryBuffer::VariableType::t_unspecified);
	//VERIFY(B.count + count < NET_PacketSizeLimit);
	//CopyMemory(&B.data[B.count], p, count);
	//B.count += count;
	//VERIFY(B.count<NET_PacketSizeLimit);
}

void NET_Packet::w_seek	(u32 pos, const void* p, u32 count)
{
	VERIFY(p && count);
	CopyMemory(Buffer->find(pos), p, count);
	//VERIFY(p && count && (pos+count<=B.count)); 
	//CopyMemory(&B.data[pos],p,count);	
//.	INI_ASSERT		(w_seek)
}

u32 NET_Packet::w_tell() { 
	return Buffer->w_pos();
	//return B.count; 
}

void NET_Packet::w_float(float a) { 
	W_guard g(&w_allow); 
	w(&a, sizeof(float));				
	INI_W(w_float(a)); 
}

void NET_Packet::w_vec3(const Fvector& a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(Fvector));
	INI_W(w_vec3(a)); 
}

void NET_Packet::w_vec4(const Fvector4& a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(Fvector4));
	INI_W(w_vec4(a)); 
}

void NET_Packet::w_u64(u64 a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(u64));				
	INI_W(w_u64(a)); 
}

void NET_Packet::w_s64(s64 a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(s64));				
	INI_W(w_s64(a)); 
}

void NET_Packet::w_u32(u32 a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(u32));				
	INI_W(w_u32(a)); 
}

void NET_Packet::w_s32(s32 a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(s32));				
	INI_W(w_s32(a)); 
}

void NET_Packet::w_u16(u16 a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(u16));				
	INI_W(w_u16(a)); 
}

void NET_Packet::w_s16(s16 a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(s16));				
	INI_W(w_s16(a)); 
}

void NET_Packet::w_u8(u8 a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(u8));				
	INI_W(w_u8(a)); 
}

void NET_Packet::w_s8(s8 a) { 
	W_guard g(&w_allow);  
	w(&a, sizeof(s8));				
	INI_W(w_s8(a)); 
}

void NET_Packet::w_float_q16(float a, float min, float max)
{
	VERIFY(a >= min && a <= max);
	float q = (a - min) / (max - min);
	w_u16(u16(iFloor(q * 65535.f + 0.5f)));
}

void NET_Packet::w_float_q8(float a, float min, float max)
{
	VERIFY(a >= min && a <= max);
	float q = (a - min) / (max - min);
	w_u8(u8(iFloor(q * 255.f + 0.5f)));
}

void NET_Packet::w_angle16(float a) { w_float_q16(angle_normalize(a), 0, PI_MUL_2); }

void NET_Packet::w_angle8(float a) { w_float_q8(angle_normalize(a), 0, PI_MUL_2); }

void NET_Packet::w_dir(const Fvector& D) { w_u16(pvCompress(D)); }

void NET_Packet::w_sdir(const Fvector& D) {
	Fvector C;
	float mag = D.magnitude();
	if (mag>EPS_S) {
		C.div(D, mag);
	}
	else {
		C.set(0, 0, 1);
		mag = 0;
	}
	w_dir(C);
	w_float(mag);
}

void NET_Packet::w_stringZ(LPCSTR S) { W_guard g(&w_allow); w(S, (u32)xr_strlen(S) + 1);	INI_W(w_stringZ(S)); }

void NET_Packet::w_stringZ(const shared_str& p)
{
	W_guard g(&w_allow);
	if (*p)
		w(*p, p.size() + 1);
	else {
		IIniFileStream* tmp = inistream;
		inistream = NULL;
		w_u8(0);
		inistream = tmp; //hack -(
	}

	INI_W(w_stringZ(p.c_str()));
}

void NET_Packet::w_matrix(Fmatrix& M)
{
	w_vec3(M.i);
	w_vec3(M.j);
	w_vec3(M.k);
	w_vec3(M.c);
}

void NET_Packet::w_clientID(ClientID& C) { w_u32(C.value()); }

void NET_Packet::w_chunk_open8(u32& position)
{
	position = w_tell();
	w_u8(0);
	INI_ASSERT(w_chunk_open8)
}

void NET_Packet::w_chunk_close8(u32 position)
{
	u32 size = u32(w_tell() - position) - sizeof(u8);
	VERIFY(size<256);
	u8			_size = (u8)size;
	w_seek(position, &_size, sizeof(_size));
	INI_ASSERT(w_chunk_close8)
}

void NET_Packet::w_chunk_open16(u32& position)
{
	position = w_tell();
	w_u16(0);
	INI_ASSERT(w_chunk_open16)
}

void NET_Packet::w_chunk_close16(u32 position)
{
	u32 size = u32(w_tell() - position) - sizeof(u16);
	VERIFY(size < 65536);
	u16			_size = (u16)size;
	w_seek(position, &_size, sizeof(_size));
	INI_ASSERT(w_chunk_close16)
}

void NET_Packet::r_seek	(u32 pos)
{
	INI_ASSERT		(r_seek)
	VERIFY(pos < Buffer->w_pos());
	//VERIFY			(pos < B.count);
	Buffer->r_seek(pos);
	//r_pos			= pos;
}

u32 NET_Packet::r_tell()	
{ 
	INI_ASSERT(r_tell)
	return Buffer->r_pos();
	//return			r_pos; 
}

void NET_Packet::r(void* p, u32 count)
{
	R_ASSERT(inistream == NULL);
	VERIFY(p && count);
	Buffer->r(p, count, MemoryBuffer::VariableType::t_unspecified);
	//CopyMemory(p, &B.data[r_pos], count);
	//r_pos += count;
	//VERIFY(r_pos <= B.count);
}

BOOL NET_Packet::r_eof()
{
	INI_ASSERT(r_eof)
	return Buffer->r_eof();
	//return			(r_pos>=B.count);
}

u32 NET_Packet::r_elapsed()
{
	INI_ASSERT(r_elapsed)
	return Buffer->w_pos() - Buffer->r_pos();
	//return			(B.count-r_pos);
}

void NET_Packet::r_advance(u32 size)		
{
	INI_ASSERT(r_advance)
	Buffer->r_advance(size);
	//r_pos			+= size;	
	//VERIFY			(r_pos<=B.count);
}

// reading - utilities
void NET_Packet::r_vec3(Fvector& A)	
{ 
	if(!inistream)
		r(&A,sizeof(Fvector));
	else
		inistream->r_vec3(A);
} // vec3

void NET_Packet::r_vec4(Fvector4& A)	
{ 
	if(!inistream)
		r(&A,sizeof(Fvector4));
	else
		inistream->r_vec4(A);
} // vec4

void NET_Packet::r_float(float& A )		
{ 
	if(!inistream)
		r	(&A,sizeof(float));						
	else
		inistream->r_float(A);
} // float

void NET_Packet::r_u64(u64& A)		
{ 
	if(!inistream)
		r(&A,sizeof(u64));
	else
		inistream->r_u64(A);
} // qword (8b)

void NET_Packet::r_s64(s64& A)		
{ 
	if(!inistream)
		r(&A,sizeof(s64));						
	else
		inistream->r_s64(A);
} // qword (8b)

void NET_Packet::r_u32(u32& A)		
{ 
	if(!inistream)
		r(&A,sizeof(u32));						
	else
		inistream->r_u32(A);
} // dword (4b)

void NET_Packet::r_s32(s32& A)		
{ 
	if(!inistream)
		r(&A,sizeof(s32));
	else
		inistream->r_s32(A);
} // dword (4b)

void NET_Packet::r_u16(u16& A)		
{ 
	if(!inistream)
		r(&A,sizeof(u16));						
	else
		inistream->r_u16(A);
} // word (2b)

void NET_Packet::r_s16(s16& A)		
{ 
	if(!inistream)
		r(&A,sizeof(s16));
	else
		inistream->r_s16(A);
} // word (2b)

void NET_Packet::r_u8(u8&  A)		
{ 
	if(!inistream)
		r(&A,sizeof(u8));
	else
		inistream->r_u8(A);
} // byte (1b)

void NET_Packet::r_s8(s8&  A)		
{ 
	if(!inistream)
		r(&A,sizeof(s8));
	else
		inistream->r_s8(A);
} // byte (1b)

// IReader compatibility
Fvector NET_Packet::r_vec3()		
{
	Fvector		A;
	r_vec3		(A);	
	return		(A);		
}

Fvector4 NET_Packet::r_vec4()
{
	Fvector4	A;
	r_vec4		(A);	
	return		(A);		
}

float NET_Packet::r_float_q8(float min,float max)
{
	float		A;
	r_float_q8	(A,min,max);
	return		A;
}

float NET_Packet::r_float_q16(float min, float max)
{
	float		A;
	r_float_q16	(A,min,max);
	return		A;
}

float NET_Packet::r_float()		
{ 
	float		A; 
	r_float		(A);					
	return		(A);		
} // float

u64 NET_Packet::r_u64()		
{ 
	u64 		A; 
	r_u64		(A);					
	return		(A);		
} // qword (8b)

s64 NET_Packet::r_s64()		
{ 
	s64 		A; 
	r_s64		(A);					
	return		(A);		
} // qword (8b)

u32 NET_Packet::r_u32()		
{ 
	u32 		A; 
	r_u32		(A);					
	return		(A);		
} // dword (4b)

s32 NET_Packet::r_s32()		
{ 
	s32			A; 
	r_s32		(A);					
	return		(A);		
} // dword (4b)

u16 NET_Packet::r_u16()		
{ 
	u16			A; 
	r_u16		(A);					
	return		(A);		
} // word (2b)

s16 NET_Packet::r_s16()		
{ 
	s16			A; 
	r_s16		(A);					
	return		(A);		
} // word (2b)

u8 NET_Packet::r_u8()		
{ 
	u8			A; 
	r_u8		(A);					
	return		(A);		
} // byte (1b)

s8 NET_Packet::r_s8()		
{ 
	s8			A; 
	r_s8		(A);					
	return		(A);		
}

void NET_Packet::r_float_q16(float& A, float min, float max)
{
	u16			val;
	r_u16		(val);
	A			= (float(val)*(max-min))/65535.f + min;		// floating-point-error possible
	VERIFY		((A >= min-EPS_S) && (A <= max+EPS_S));
}

void NET_Packet::r_float_q8(float& A, float min, float max)
{
	u8			val;
	r_u8		(val);
	A			= (float(val)/255.0001f) *(max-min) + min;	// floating-point-error possible
	VERIFY		((A >= min) && (A <= max));
}

void NET_Packet::r_angle16(float& A)		
{ 
	r_float_q16	(A,0,PI_MUL_2);	
}

void NET_Packet::r_angle8(float& A)		
{ 
	r_float_q8	(A,0,PI_MUL_2);	
}

void NET_Packet::r_dir(Fvector& A)	
{ 
	u16			t; 
	r_u16		(t); 
	pvDecompress(A,t); 
}

void NET_Packet::r_sdir(Fvector& A)
{
	u16				t;	
	float			s;
	r_u16			(t);
	r_float			(s);
	pvDecompress	(A,t);
	A.mul			(s);
}

void NET_Packet::r_stringZ( LPSTR S )
{
 	if(!inistream)
	{
		LPCSTR data = LPCSTR(Buffer->find(Buffer->r_pos()));
		//LPCSTR	data	= LPCSTR(&B.data[r_pos]);
		size_t	len		= xr_strlen(data);
		r				(S,(u32)len+1);
	}else{
		inistream->r_string(S, 4096);//???
	}
}

void NET_Packet::r_stringZ( xr_string& dest )
{
 	if(!inistream)
	{
		dest = LPCSTR(Buffer->find(Buffer->r_pos()));
		//dest			= LPCSTR(&B.data[r_pos]);
		r_advance		(u32(dest.size()+1));
	}else{
		string4096		buff;
		inistream->r_string(buff, sizeof(buff));
		dest			= buff;
	}
}

void NET_Packet::r_stringZ(shared_str& dest)
{
 	if(!inistream)
	{
		dest = LPCSTR(Buffer->find(Buffer->r_pos()));
		//dest			= LPCSTR(&B.data[r_pos]);
		r_advance		(dest.size()+1);
	}else{
		string4096		buff;
		inistream->r_string(buff, sizeof(buff));
		dest			= buff;
	}
}

void NET_Packet::skip_stringZ()
{
	if (!inistream)
	{
		LPCSTR data = LPCSTR(Buffer->find(Buffer->r_pos()));
		//LPCSTR	data	= LPCSTR(&B.data[r_pos]);
		u32	len		= xr_strlen(data);
		r_advance		(len + 1);
	} else {
		inistream->skip_stringZ	();
	}
}

void NET_Packet::r_matrix(Fmatrix& M)
{
	r_vec3	(M.i);	M._14_	= 0;
	r_vec3	(M.j);	M._24_	= 0;
	r_vec3	(M.k);	M._34_	= 0;
	r_vec3	(M.c);	M._44_	= 1;
}

void NET_Packet::r_clientID(ClientID& C)
{
	u32				tmp;
	r_u32			(tmp);
	C.set			(tmp);
}

void NET_Packet::r_stringZ_s	(LPSTR string, u32 const size)
{
	if ( inistream ) {
		inistream->r_string( string, size );
		return;
	}

	LPCSTR data = LPCSTR(Buffer->find(Buffer->r_pos()));
	//LPCSTR data		= LPCSTR( B.data + r_pos );
	u32 length		= xr_strlen( data );
	R_ASSERT2		( ( length+1 ) <= size, "buffer overrun" );
	r				( string, length+1 );
}

NET_Packet NET_Packet_Wrapper::CreateNetwork()
{
	NET_Packet packet = NET_Packet();
	packet.Buffer = xr_make_shared<MemoryBuffer::CBufferSimple>();
	return packet;
}

NET_Packet* NET_Packet_Wrapper::CreateNetworkPtr()
{
	NET_Packet* packet = new NET_Packet();
	packet->Buffer = xr_make_shared<MemoryBuffer::CBufferSimple>();
	return packet;
}

NET_Packet NET_Packet_Wrapper::CreateSerialize()
{
	NET_Packet packet = NET_Packet();
	packet.Buffer = xr_make_shared<MemoryBuffer::CBufferComplex>();
	return packet;
}
