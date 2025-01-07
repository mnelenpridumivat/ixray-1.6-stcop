#include "stdafx.h"
#include "SaveChunk.h"
#include "MemoryBuffer.h"
#include "SaveManager.h"

CSaveChunk::~CSaveChunk()
{
	for (auto& elem : _subchunks) {
		xr_delete(elem.second);
	}
	for (size_t i = 0; i < _variables.size(); ++i) {
		xr_delete(_variables[i]);
	}
}

void CSaveChunk::Write(CMemoryBuffer& Buffer)
{
	Buffer.Write((u8)ESaveVariableType::t_chunkStart);
	CSaveManager::GetInstance().ConditionalWriteString(_chunkName, Buffer);
	for (const auto& elem : _subchunks) {
		elem.second->Write(Buffer);
	}
	for (const auto& elem : _variables) {
		elem->Write(Buffer);
	}
	Buffer.Write((u8)ESaveVariableType::t_chunkEnd);
}

void CSaveChunk::ReadArray(u64& Size)
{
	if (_currentArrayStack.empty()) {
		auto Array = smart_cast<ISaveVariableArray*>(_variables[_currentReadIndex++]);
		VERIFY(Array);
		_currentArrayStack.push(Array);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		auto CurrentElement = smart_cast<ISaveVariableArray*>(CurrentArray->GetCurrentElement());
		VERIFY(CurrentElement);
		_currentArrayStack.push(CurrentElement);
	}
	Size = _currentArrayStack.top()->GetSize();
}

void CSaveChunk::WriteArray()
{
	_variables.emplace_back(new CSaveVariableArrayUnspec());
	auto ArrayPtr = (ISaveVariableArray*)_variables.back();
	_currentArrayStack.push(ArrayPtr);

}

void CSaveChunk::EndArray()
{
	_currentArrayStack.pop();
}

CSaveChunk* CSaveChunk::BeginChunk(shared_str ChunkName)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_subchunks.find(ChunkName) == _subchunks.end());
		return _subchunks.emplace(ChunkName, new CSaveChunk(ChunkName)).first->second;
	}
	else {
		auto Array = _currentArrayStack.top();
		auto Value = new CSaveChunk(ChunkName);
		Array->AddVariable(Value);
		return Value;
	}
}

CSaveChunk* CSaveChunk::FindChunk(shared_str ChunkName)
{
	if (_currentArrayStack.empty()) {
		auto Chunk = _subchunks.find(ChunkName);
		VERIFY(Chunk != _subchunks.end());
		return Chunk->second;
	}
	else {
		auto Array = _currentArrayStack.top();
		auto CurrentElement = Array->GetCurrentElement(); 
		Array->Next();
		VERIFY(CurrentElement->GetVariableType() == ESaveVariableType::t_chunk);
		auto CastedElement = (CSaveChunk*)CurrentElement;
		VERIFY(CastedElement->_chunkName == ChunkName);
		return CastedElement;
	}
}

void CSaveChunk::w_bool(bool a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableBool(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableBool(a));
	}
}

void CSaveChunk::w_float(float a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableFloat(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableFloat(a));
	}
}

void CSaveChunk::w_double(double a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableDouble(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableDouble(a));
	}
}

void CSaveChunk::w_u64(u64 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableU64(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableU64(a));
	}
}

void CSaveChunk::w_s64(s64 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableS64(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableS64(a));
	}
}

void CSaveChunk::w_u32(u32 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableU32(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableU32(a));
	}
}

void CSaveChunk::w_s32(s32 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableS32(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableS32(a));
	}
}

void CSaveChunk::w_u16(u16 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableU16(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableU16(a));
	}
}

void CSaveChunk::w_s16(s16 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableS16(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableS16(a));
	}
}

void CSaveChunk::w_u8(u8 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableU8(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableU8(a));
	}
}

void CSaveChunk::w_s8(s8 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableS8(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableS8(a));
	}
}

void CSaveChunk::w_string(shared_str S)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableString(S));
	}
	else {
		((ISaveVariableArray*)_currentArrayStack.top())->AddVariable(new CSaveVariableString(S));
	}
}

void CSaveChunk::r_bool(bool& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<bool, CSaveVariableBool>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<bool, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_float(float& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_float);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableFloat>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_float);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableFloat>((CSaveVariableFloat*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_double(double& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_double);
		A = SSaveVariableGetter::GetValue<double, CSaveVariableDouble>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_double);
		A = SSaveVariableGetter::GetValue<double, CSaveVariableDouble>((CSaveVariableDouble*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_u64(u64& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_u64);
		A = SSaveVariableGetter::GetValue<u64, CSaveVariableU64>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_u64);
		A = SSaveVariableGetter::GetValue<u64, CSaveVariableU64>((CSaveVariableU64*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_s64(s64& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_s64);
		A = SSaveVariableGetter::GetValue<s64, CSaveVariableS64>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_s64);
		A = SSaveVariableGetter::GetValue<s64, CSaveVariableS64>((CSaveVariableS64*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_u32(u32& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_u32);
		A = SSaveVariableGetter::GetValue<u32, CSaveVariableU32>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_u32);
		A = SSaveVariableGetter::GetValue<u32, CSaveVariableU32>((CSaveVariableU32*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_s32(s32& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_s32);
		A = SSaveVariableGetter::GetValue<s32, CSaveVariableS32>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_s32);
		A = SSaveVariableGetter::GetValue<s32, CSaveVariableS32>((CSaveVariableS32*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_u16(u16& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_u16);
		A = SSaveVariableGetter::GetValue<u16, CSaveVariableU16>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_u16);
		A = SSaveVariableGetter::GetValue<u16, CSaveVariableU16>((CSaveVariableU16*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_s16(s16& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_s16);
		A = SSaveVariableGetter::GetValue<s16, CSaveVariableS16>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_s16);
		A = SSaveVariableGetter::GetValue<s16, CSaveVariableS16>((CSaveVariableS16*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_u8(u8& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_u8);
		A = SSaveVariableGetter::GetValue<u8, CSaveVariableU8>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_u8);
		A = SSaveVariableGetter::GetValue<u8, CSaveVariableU8>((CSaveVariableU8*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_s8(s8& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_s8);
		A = SSaveVariableGetter::GetValue<s8, CSaveVariableS8>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_s8);
		A = SSaveVariableGetter::GetValue<s8, CSaveVariableS8>((CSaveVariableS8*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_string(shared_str& S)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_string);
		S = SSaveVariableGetter::GetValue<shared_str, CSaveVariableString>(_variables[_currentReadIndex++]).c_str();
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_string);
		S = SSaveVariableGetter::GetValue<shared_str, CSaveVariableString>((CSaveVariableBool*)CurrentArray->GetCurrentElement()).c_str();
		CurrentArray->Next();
	}
}

void CSaveChunk::Parse(IReader* stream)
{
	{
		ESaveVariableType type;
		stream->r(&type, sizeof(ESaveVariableType));
		while(type == ESaveVariableType::t_chunkStart) {
			shared_str subchunk_name;
			CSaveManager::GetInstance().ConditionalReadString(stream, subchunk_name);
			CSaveChunk* NewChunk = new CSaveChunk(subchunk_name);
			NewChunk->Parse(stream);
			_subchunks[subchunk_name] = NewChunk;
			stream->r(&type, sizeof(ESaveVariableType));
		}
		ParseRec(stream, type);
	}
}

void CSaveChunk::ParseRec(IReader* stream, ESaveVariableType type_key)
{
	ESaveVariableType type = type_key;
	while (type != ESaveVariableType::t_chunkEnd) {
		switch (type)
		{
		case ESaveVariableType::t_chunkStart: {
			if (_currentArrayStack.empty()) {
				Msg("_currentArrayStack.empty()");
			}
			//VERIFY(!_currentArrayStack.empty());
			shared_str subchunk_name;
			CSaveManager::GetInstance().ConditionalReadString(stream, subchunk_name);
			CSaveChunk* NewChunk = new CSaveChunk(subchunk_name);
			NewChunk->Parse(stream);
			VERIFY(!_currentArrayStack.empty());
			_currentArrayStack.top()->AddVariable(NewChunk);
			break;
		}
		case ESaveVariableType::t_bool: {
			bool Value;
			CSaveManager::GetInstance().ConditionalReadBool(stream, Value);
			auto Var = new CSaveVariableBool(Value);
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_float: {
			auto Var = new CSaveVariableFloat(stream->r_float());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_double: {
			double Value;
			stream->r(&Value, sizeof(double));
			auto Var = new CSaveVariableFloat(stream->r_float());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u64: {
			auto Var = new CSaveVariableU64(stream->r_u64());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u64_op32: {
			auto Var = new CSaveVariableU64(stream->r_u32());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u64_op16: {
			auto Var = new CSaveVariableU64(stream->r_u16());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u64_op8: {
			auto Var = new CSaveVariableU64(stream->r_u8());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s64: {
			auto Var = new CSaveVariableS64(stream->r_s64());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s64_op32: {
			auto Var = new CSaveVariableS64(stream->r_s32());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s64_op16: {
			auto Var = new CSaveVariableS64(stream->r_s16());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s64_op8: {
			auto Var = new CSaveVariableS64(stream->r_s8());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u32: {
			auto Var = new CSaveVariableU32(stream->r_u32());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u32_op16: {
			auto Var = new CSaveVariableU32(stream->r_u16());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u32_op8: {
			auto Var = new CSaveVariableU32(stream->r_u8());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s32: {
			auto Var = new CSaveVariableS32(stream->r_s32());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s32_op16: {
			auto Var = new CSaveVariableS32(stream->r_s16());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s32_op8: {
			auto Var = new CSaveVariableS32(stream->r_s8());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u16: {
			auto Var = new CSaveVariableU16(stream->r_u16());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u16_op8: {
			auto Var = new CSaveVariableU16(stream->r_u8());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s16: {
			auto Var = new CSaveVariableS16(stream->r_s16());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s16_op8: {
			auto Var = new CSaveVariableS16(stream->r_s8());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_u8: {
			auto Var = new CSaveVariableU8(stream->r_u8());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_s8: {
			auto Var = new CSaveVariableS8(stream->r_s8());
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		case ESaveVariableType::t_string: {
			shared_str Value;
			CSaveManager::GetInstance().ConditionalReadString(stream, Value);
			auto Var = new CSaveVariableString(Value);
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			break;
		}
		/*case ESaveVariableType::t_array: {
			size_t Size;
			stream->r(&Size, sizeof(size_t));
			auto Var = new CSaveVariableArray(Size);
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			_currentArrayStack.push(Var);
			for (u64 i = 0; i < Size; ++i) {
				stream->r(&type, sizeof(ESaveVariableType));
				ParseRec(stream, type);
			}
			_currentArrayStack.pop();
			break;
		}*/
		case ESaveVariableType::t_arrayUnspec: {
			auto Var = new CSaveVariableArrayUnspec();
			if (_currentArrayStack.empty()) {
				_variables.emplace_back(Var);
			}
			else {
				_currentArrayStack.top()->AddVariable(Var);
			}
			_currentArrayStack.push(Var);
			stream->r(&type, sizeof(ESaveVariableType));
			while (type != ESaveVariableType::t_arrayUnspecEnd) {
				ParseRec(stream, type);
				stream->r(&type, sizeof(ESaveVariableType));
			}
			_currentArrayStack.pop();
			break;
		}
		default: {
			FATAL("Invalid save chunk type!");
		}
		}
		{
			auto Pos = stream->tell();
			stream->r(&type, sizeof(ESaveVariableType));
			if (type == ESaveVariableType::t_arrayUnspecEnd) {
				stream->seek(Pos);
				return;
			}
		}
	}
}
