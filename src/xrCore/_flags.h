#pragma once
#include "Save/SaveInterface.h"

template <class T>
struct _flags {
public:
	typedef T			TYPE;
	typedef _flags<T>	Self;
	typedef Self&		SelfRef;
	typedef const Self&	SelfCRef;
public:
	T 	flags;

	//IC	bool	operator==(SelfCRef Left)
	//{
	//	return Left.flags == flags;
	//}

    IC	TYPE	get		()									const	{ return flags;}
    IC	SelfRef	zero	()											{ flags=T(0);	return *this;	}
    IC	SelfRef	one		()											{ flags=T(-1);	return *this;	}
    IC	SelfRef	invert	()											{ flags	=	~flags;		return *this;	}
    IC	SelfRef	invert	(const Self& f)								{ flags	=	~f.flags;	return *this;	}
    IC	SelfRef	invert	(const T mask)								{ flags ^=	mask;		return *this;	}
	IC	SelfRef	assign	(const Self& f)								{ flags =	f.flags;	return *this;	}
	IC	SelfRef	assign	(const T mask)								{ flags	=	mask;		return *this;	}
	IC	SelfRef	set		(const T mask,	BOOL value)					{ if (value) flags|=mask; else flags&=~mask; return *this; }
	IC 	BOOL	is		(const T mask)						const	{ return mask==(flags&mask);			}
#if defined(IXR_WINDOWS) && !defined(_M_X64)
	IC  bool    bitTest(const int bitNum)					const { u32 tempFlag = flags; return _bittest((long*)&tempFlag, bitNum); }
#else
	IC  bool    bitTest(const int bitNum)					const { u64 tempFlag = flags; return _bittest64((s64*)&tempFlag, bitNum); }
#endif

	IC 	BOOL	is_any	(const T mask)						const	{ return BOOL(!!(flags&mask));			}
	IC 	BOOL	test	(const T mask)						const	{ return BOOL(!!(flags&mask));			}
	IC 	SelfRef	bor		(const T mask)								{ flags|=mask;			return *this;	}
	IC 	SelfRef	bor		(const Self& f, const T mask) 				{ flags=f.flags|mask;	return *this;	}
	IC 	SelfRef	band		(const T mask)								{ flags&=mask;			return *this;	}
	IC 	SelfRef	band		(const Self& f, const T mask) 				{ flags=f.flags&mask;	return *this;	}
	IC 	BOOL	equal	(const Self& f) 			  		const	{ return flags==f.flags;}
	IC 	BOOL	equal	(const Self& f, const T mask) 		const	{ return (flags&mask)==(f.flags&mask);}
};

typedef _flags<u8>	Flags8;		typedef _flags<u8>	flags8;		
typedef _flags<u16>	Flags16;	typedef _flags<u16>	flags16;
typedef _flags<u32>	Flags32;	typedef _flags<u32>	flags32;
typedef _flags<u64>	Flags64;	typedef _flags<u64>	flags64;

template <class T>
bool operator == (_flags<T> const& A, _flags<T>  const& B) { return A.flags == B.flags; }

template<typename T> ISaveObject& operator<<(ISaveObject& Object, _flags<T>& Value) {
	return Object << Value.flags;
}

template<size_t T>
struct _flags_big {
	typedef _flags_big<T>	Self;
	typedef Self& SelfRef;
	typedef const Self& SelfCRef;
	static constexpr size_t ArrSize = T / (sizeof(size_t)*8) + (T % (sizeof(size_t)*8) > 0);

private:
	Flags64 Bits[ArrSize];

public:

	IC static constexpr size_t GetChunkNumber(size_t BitPos) {
		return BitPos / (sizeof(size_t) * 8) + (BitPos % (sizeof(size_t) * 8) > 0);
	}

	IC static constexpr u64 GetBitNumber(size_t BitPos) {
		return BitPos % (sizeof(size_t) * 8);
	}

	IC static constexpr u64 GetBitMask(size_t BitPos) {
		return u64(1) << GetBitNumber(BitPos);
	}

	IC	SelfRef	zero() { 
		for (size_t i = 0; i < ArrSize; ++i) {
			Bits[i].zero();
		}
		return *this; 
	}

	IC	SelfRef	one() {
		for (size_t i = 0; i < ArrSize; ++i) {
			Bits[i].one();
		}
		return *this; 
	}

	IC	SelfRef	invert() {
		for (size_t i = 0; i < ArrSize; ++i) {
			Bits[i].invert();
		}
		return *this; 
	}

	IC	SelfRef	invert(const Self& f) {
		for (size_t i = 0; i < ArrSize; ++i) {
			Bits[i].invert(f.Bits[i]);
		}
		return *this; 
	}

	IC	SelfRef	invert(const u64 mask, size_t chunkNumber) { 
		VERIFY(chunkNumber < ArrSize);
		Bits[chunkNumber].invert(mask);
		return *this; 
	}

	IC	SelfRef	assign(const Self& f) {
		for (size_t i = 0; i < ArrSize; ++i) {
			Bits[i].assign(f.Bits[i]);
		}
		return *this; 
	}

	IC	SelfRef	assign(const u64 mask, size_t chunkNumber) {
		VERIFY(chunkNumber < ArrSize);
		Bits[chunkNumber].assign(mask);
		return *this; 
	}

	IC	SelfRef	set(const u64 mask, bool value, size_t chunkNumber) {
		VERIFY(chunkNumber < ArrSize);
		Bits[chunkNumber].set(mask, value);
		return *this; 
	}

	IC 	bool	is(const u64 mask, size_t chunkNumber)						const {
		VERIFY(chunkNumber < ArrSize);
		return Bits[chunkNumber].is(mask);
	}
#if defined(IXR_WINDOWS) && !defined(_M_X64)
	IC  bool    bitTest(const u64 bitNum) const {
		return _bittest((s32*)&Bits[GetChunkNumber(bitNum)].flags, bitNum);
		//u32 tempFlag = flags; 
		//return _bittest((long*)&tempFlag, bitNum); 
	}

#else
	IC  bool    bitTest(const u64 bitNum) const { 
		return _bittest64((s64*)&Bits[GetChunkNumber(bitNum)].flags, bitNum);
		//u64 tempFlag = flags; 
		//return _bittest64((s64*)&tempFlag, bitNum); 
	}
#endif

	IC 	bool	is_any(const u64 mask, size_t chunkNumber)						const {
		VERIFY(chunkNumber < ArrSize);
		return Bits[chunkNumber].is_any(mask);
	
	}

	IC 	bool	is_any(const u64 mask)						const {
		for (size_t i = 0; i < ArrSize; ++i) {
			if (Bits[i].is_any(mask)) {
				return true;
			}
		}
		return false;

	}

	IC 	bool	test(const u64 mask, size_t chunkNumber)						const {
		VERIFY(chunkNumber < ArrSize);
		return Bits[chunkNumber].test(mask);
	
	}

	IC 	SelfRef	bor(const u64 mask, size_t chunkNumber) {
		VERIFY(chunkNumber < ArrSize);
		Bits[chunkNumber].bor(mask);
		return *this; 
	}

	IC 	SelfRef	bor(const Self& f, const u64 mask, size_t chunkNumber) {
		VERIFY(chunkNumber < ArrSize);
		Bits[chunkNumber].bor(f.Bits[chunkNumber].flags|mask);
		return *this; 
	}

	IC 	SelfRef	bor(const Self& f) {
		for (size_t i = 0; i < ArrSize; ++i) {
			Bits[i].bor(f.Bits[i].flags);
		}
		return *this;
	}

	IC 	SelfRef	band(const u64 mask, size_t chunkNumber) {
		VERIFY(chunkNumber < ArrSize);
		Bits[chunkNumber].band(mask);
		return *this; 
	}

	IC 	SelfRef	band(const Self& f, const u64 mask, size_t chunkNumber) {
		VERIFY(chunkNumber < ArrSize);
		Bits[chunkNumber].band(f.Bits[chunkNumber].flags & mask);
		return *this; 
	}

	IC 	SelfRef	band(const Self& f) {
		for (size_t i = 0; i < ArrSize; ++i) {
			Bits[i].band(f.Bits[i].flags);
		}
		return *this; 
	}

	IC 	bool	equal(const Self& f) 			  		const {
		bool IsEqual = true;
		for (size_t i = 0; i < ArrSize; ++i) {
			IsEqual = IsEqual && Bits[i].equal(f.Bits[i]);
		}
		return IsEqual;
	}

	IC 	bool	equal(const Self& f, const u64 mask, size_t chunkNumber) 		const {
		VERIFY(chunkNumber < ArrSize);
		return Bits[chunkNumber].equal(f.Bits[chunkNumber]);
	}

	IC	void set_all()
	{
		for (size_t i = 0; i < ArrSize; ++i) {
			Bits[i].one();
		}
	}

	IC	size_t count() 
	{
		size_t Result = 0;
		for (size_t i = 0; i < T; ++i) {
			Result += bitTest(i);
		}
		return Result;
	}

	IC u64 GetRawChunkData(u64 chunkNumber) const {
		VERIFY(chunkNumber < ArrSize);
		return Bits[chunkNumber].flags;
	}

	IC bool operator!=	(SelfCRef _second) const noexcept
	{
		for (size_t i = 0; i < ArrSize; ++i) {
			if (Bits[i] != _second.Bits[i]) {
				return true;
			}
		}

		return false;
	}

	IC bool operator==	(SelfCRef _second) const noexcept
	{
		for (size_t i = 0; i < ArrSize; ++i) {
			if (Bits[i] != _second.Bits[i]) {
				return false;
			}
		}

		return true;
	}

	_flags_big() {
		zero();
	}

	IC SelfRef operator=	(SelfCRef _second)
	{
		std::memcpy(Bits, _second.Bits, ArrSize*sizeof(Flags64));
		return *this;
	}
};

typedef _flags_big<128>	 Flags128;	typedef _flags_big<128>	 flags128;
typedef _flags_big<256>	 Flags256;	typedef _flags_big<256>	 flags256;
typedef _flags_big<512>	 Flags512;	typedef _flags_big<512>	 flags512;
typedef _flags_big<1024> Flags1024;	typedef _flags_big<1024> flags1024;

template<size_t T> ISaveObject& operator<<(ISaveObject& Object, _flags_big<T>& Value) {
	if (!Object.IsSave()) {
		Value.zero();
	}
	for (u64 i = 0; i < _flags_big<T>::ArrSize; ++i) {
		if (Object.IsSave()) {
			auto ChunkNumber = Value.GetRawChunkData(i);
			Object << ChunkNumber;
		}
		else {
			Flags64 temp;
			Object << temp;
			Value.zero();
			Value.bor(temp.flags, i);
		}
	}
	return Object;
}
