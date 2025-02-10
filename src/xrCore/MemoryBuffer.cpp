#include "stdafx.h"
#include "MemoryBuffer.h"

void MemoryBuffer::CBufferSimple::w_start()
{
	count = 0;
}

u32 MemoryBuffer::CBufferSimple::w_pos()
{
	return count;
}

void MemoryBuffer::CBufferSimple::w(const void* p, u32 count, VariableType type)
{
	VERIFY(p && count);
	VERIFY(this->count + count < NET_PacketSizeLimit);
	CopyMemory(&data[this->count], p, count);
	this->count += count;
	VERIFY(this->count < NET_PacketSizeLimit);
}

void MemoryBuffer::CBufferSimple::r_start()
{
	r_position = 0;
}

u32 MemoryBuffer::CBufferSimple::r_pos()
{
	return r_position;
}

void MemoryBuffer::CBufferSimple::r(void* p, u32 count, VariableType type)
{
	VERIFY(p && count);
	CopyMemory(p, &data[r_position], count);
	r_position += count;
	VERIFY(r_position <= this->count);
}

void* MemoryBuffer::CBufferSimple::find(u32 count)
{
	VERIFY(count < this->count);
	return &data[count];
}

bool MemoryBuffer::CBufferSimple::r_eof()
{
	return r_position >= count;
}

void MemoryBuffer::CBufferSimple::r_seek(u32 pos)
{
	R_ASSERT(pos < count);
	r_position = pos;
}

void MemoryBuffer::CBufferSimple::r_advance(u32 count)
{
	r_position += count;
	R_ASSERT(r_position + count < this->count);
}

void* MemoryBuffer::CBufferSimple::get_raw()
{
	return &data;
}

void MemoryBuffer::CBufferSimple::write_data(IWriter& writer)
{
	writer.w_u16(u16(w_pos()));
	writer.w(get_raw(), w_pos());
}

void MemoryBuffer::CBufferSimple::read_data(IReader& reader)
{
	count = reader.r_u32();
	reader.r(data, count);
}

u32 MemoryBuffer::CBufferComplex::SearchInternalPos(u32 pos)
{
	auto RawIndex = (u32)(count / ((double)this->count / Variables.size())); // predict search element index
	VERIFY(RawIndex < Variables.size());
	s8 sign = 0;
	while (Variables[RawIndex].first != count) { // if prediction wrong, try to find real position
		s64 diff = Variables[RawIndex].first - count;
		R_ASSERT(sign && diff * sign != abs(diff)); // verification if not looped
		if (diff) {
			--RawIndex;
		}
		else {
			++RawIndex;
		}
		sign = diff > 0 ? 1 : -1;
	}
	return RawIndex;
}

void MemoryBuffer::CBufferComplex::w_start()
{
	count = 0;
	Variables.erase(Variables.begin(), Variables.end());
}

u32 MemoryBuffer::CBufferComplex::w_pos()
{
	return count;
}

void MemoryBuffer::CBufferComplex::w(const void* p, u32 count, VariableType type)
{
	VERIFY(p && count);
	Variables.push_back(xr_pair<u32, Content>());
	auto& back = Variables.back();
	back.first = this->count;
	back.second.first = type;
	back.second.second = xr_make_unique<MemoryBuffer::CVariable>(p, count);
	this->count += count;
}

void MemoryBuffer::CBufferComplex::r_start()
{
	r_position = 0;
	r_pos_internal = 0;
}

u32 MemoryBuffer::CBufferComplex::r_pos()
{
	return r_position;
}

void MemoryBuffer::CBufferComplex::r(void* p, u32 count, VariableType type)
{
	VERIFY(p && count);
	VERIFY(r_pos_internal < Variables.size());
	auto& elem = Variables[r_pos_internal++];
	VERIFY(elem.first == r_position);
	R_ASSERT(elem.second.first == type);
	CopyMemory(p, elem.second.second->GetPtr(), count);
	r_position += count;
}

void* MemoryBuffer::CBufferComplex::find(u32 count)
{
	VERIFY(count < this->count);
	return Variables[SearchInternalPos(count)].second.second->GetPtr();
}

bool MemoryBuffer::CBufferComplex::r_eof()
{
	return r_position >= count;
}

void MemoryBuffer::CBufferComplex::r_seek(u32 pos)
{
	R_ASSERT(pos < count);
	r_position = pos;
	r_pos_internal = SearchInternalPos(pos);
}

void MemoryBuffer::CBufferComplex::r_advance(u32 count)
{
	R_ASSERT(r_position + count < this->count);
	r_position += count;
	r_pos_internal = SearchInternalPos(r_position);
}

void* MemoryBuffer::CBufferComplex::get_raw()
{
	FATAL("Attempt to access raw data of complex memory buffer!");
	return nullptr;
}

void MemoryBuffer::CBufferComplex::write_data(IWriter& writer)
{
	writer.w_u32(w_pos());
	for (const auto& elem : Variables) {
		writer.w(&elem.second.first, sizeof(MemoryBuffer::VariableType));
		writer.w(elem.second.second->GetPtr(), elem.second.second->GetSize());
	}
}

void MemoryBuffer::CBufferComplex::read_data(IReader& reader)
{
	count = reader.r_u32();
	for (u32 i = 0; i < count; ++i) {
		VariableType type = VariableType::t_unspecified;
		reader.r(&type, sizeof(VariableType));
		Variables[count].second.first = type;
		Variables[count].second.second = xr_make_unique<CVariable>(type, reader);
		count += VariablesSize.find(type)->second;
	}
}

MemoryBuffer::CVariable::CVariable(const void* p, u32 size)
{
	this->size = size;
	Value.resize(size, 0);
	CopyMemory(GetPtr(), p, size);
}

MemoryBuffer::CVariable::CVariable(VariableType type, IReader& stream)
{
	size = VariablesSize.find(type)->second;
	Value.resize(size, 0);
	stream.r(GetPtr(), size);
}

void MemoryBuffer::CVariable::SetValue(const void* p, u32 size)
{
	VERIFY(Value.size() == size);
	CopyMemory(GetPtr(), p, size);
}
