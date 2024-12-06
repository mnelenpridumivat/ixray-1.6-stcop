#include "stdafx.h"
#include "SaveChunk.h"

CSaveChunk::~CSaveChunk()
{
	for (auto& elem : _subchunks) {
		xr_delete(elem.second);
	}
	for (size_t i = 0; i < _variables.size(); ++i) {
		xr_delete(_variables[i]);
	}
}

void CSaveChunk::ReadArray(u64& Size)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->IsArray());
		_currentArrayStack.push((CSaveVariableArray*)_variables[_currentReadIndex]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->IsArray());
		_currentArrayStack.push((CSaveVariableArray*)CurrentArray);
	}
	Size = _currentArrayStack.top()->GetSize();
}

void CSaveChunk::WriteArray(u64 Size)
{
	_variables.emplace_back(new CSaveVariableArray(Size));
	auto ArrayPtr = (CSaveVariableArray*)_variables.back();
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

void CSaveChunk::w_vec3(const Fvector& a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableVec3(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableVec3(a));
	}
}

void CSaveChunk::w_vec4(const Fvector4& a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableVec4(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableVec4(a));
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

/*void CSaveChunk::w_float_q16(float a, float min, float max)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableFloatQ16>(a, min, max));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableFloatQ16(a, min, max));
	}
}

void CSaveChunk::w_float_q8(float a, float min, float max)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableFloatQ8>(a, min, max));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableFloatQ8(a, min, max));
	}
}

void CSaveChunk::w_angle16(float a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableAngle16>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableAngle16(a));
	}
}

void CSaveChunk::w_angle8(float a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableAngle8>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableAngle8(a));
	}
}

void CSaveChunk::w_dir(const Fvector& D)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableDir>(D));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableDir(D));
	}
}

void CSaveChunk::w_sdir(const Fvector& D)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableSdir>(D));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableSdir(D));
	}
}*/

void CSaveChunk::w_stringZ(LPCSTR S)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableString(S));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableString(S));
	}
}

void CSaveChunk::w_stringZ(const shared_str& p)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableString(p));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableString(p));
	}
}

void CSaveChunk::w_stringZ(const xr_string& p)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableString(p));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableString(p));
	}
}

void CSaveChunk::w_matrix(Fmatrix& M)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableMatrix(M));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableMatrix(M));
	}
}

void CSaveChunk::w_clientID(ClientID& C)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(new CSaveVariableClientID(C));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableClientID(C));
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

void CSaveChunk::r_vec3(Fvector& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_vec3);
		A = SSaveVariableGetter::GetValue<Fvector, CSaveVariableVec3>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_vec3);
		A = SSaveVariableGetter::GetValue<Fvector, CSaveVariableVec3>((CSaveVariableVec3*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_vec4(Fvector4& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_vec4);
		A = SSaveVariableGetter::GetValue<Fvector4, CSaveVariableVec4>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_vec4);
		A = SSaveVariableGetter::GetValue<Fvector4, CSaveVariableVec4>((CSaveVariableVec4*)CurrentArray->GetCurrentElement());
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

/*void CSaveChunk::r_float_q16(float& A, float min, float max)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_float_q16);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableFloatQ16>(_variables[_currentReadIndex++].get());
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_float_q16);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableFloatQ16>((CSaveVariableFloatQ16*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_float_q8(float& A, float min, float max)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableBool>(_variables[_currentReadIndex++].get());
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_angle16(float& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableBool>(_variables[_currentReadIndex++].get());
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_angle8(float& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableBool>(_variables[_currentReadIndex++].get());
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<float, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_dir(Fvector& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<Fvector, CSaveVariableBool>(_variables[_currentReadIndex++].get());
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<Fvector, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_sdir(Fvector& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<Fvector, CSaveVariableBool>(_variables[_currentReadIndex++].get());
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<Fvector, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}*/

void CSaveChunk::r_stringZ(LPSTR S)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_string);
		auto TempStr = SSaveVariableGetter::GetValue<xr_string, CSaveVariableBool>(_variables[_currentReadIndex++]);
		xr_strcpy(S, TempStr.size(), TempStr.c_str());
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_string);
		auto TempStr = SSaveVariableGetter::GetValue<xr_string, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		xr_strcpy(S, TempStr.size(), TempStr.c_str());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_stringZ(xr_string& dest)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_string);
		dest = SSaveVariableGetter::GetValue<xr_string, CSaveVariableBool>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_string);
		dest = SSaveVariableGetter::GetValue<xr_string, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_stringZ(shared_str& dest)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_string);
		dest = SSaveVariableGetter::GetValue<xr_string, CSaveVariableBool>(_variables[_currentReadIndex++]).c_str();
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_string);
		dest = SSaveVariableGetter::GetValue<xr_string, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement()).c_str();
		CurrentArray->Next();
	}
}

void CSaveChunk::r_stringZ_s(LPSTR string, u32 size)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_string);
		auto TempStr = SSaveVariableGetter::GetValue<xr_string, CSaveVariableBool>(_variables[_currentReadIndex++]);
		VERIFY(TempStr.size() <= size);
		xr_strcpy(string, TempStr.size(), TempStr.c_str());
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_string);
		auto TempStr = SSaveVariableGetter::GetValue<xr_string, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		VERIFY(TempStr.size() <= size);
		xr_strcpy(string, TempStr.size(), TempStr.c_str());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_matrix(Fmatrix& M)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_matrix);
		M = SSaveVariableGetter::GetValue<Fmatrix, CSaveVariableMatrix>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_matrix);
		M = SSaveVariableGetter::GetValue<Fmatrix, CSaveVariableMatrix>((CSaveVariableMatrix*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}

void CSaveChunk::r_clientID(ClientID& C)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_clientID);
		C = SSaveVariableGetter::GetValue<ClientID, CSaveVariableClientID>(_variables[_currentReadIndex++]);
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_clientID);
		C = SSaveVariableGetter::GetValue<ClientID, CSaveVariableClientID>((CSaveVariableClientID*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}
