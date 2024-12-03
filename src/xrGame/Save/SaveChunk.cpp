#include "StdAfx.h"
#include "SaveChunk.h"

void CSaveChunk::ReadArray(u64& Size)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->IsArray());
		_currentArrayStack.push((CSaveVariableArray*)_variables[_currentReadIndex].get());
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
	_variables.emplace_back(xr_make_unique<CSaveVariableArray>(Size));
	auto ArrayPtr = (CSaveVariableArray*)_variables.back().get();
	_currentArrayStack.push(ArrayPtr);

}

void CSaveChunk::EndArray()
{
	_currentArrayStack.pop();
}

void CSaveChunk::w_bool(bool a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableBool>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableBool(a));
	}
}

void CSaveChunk::w_float(float a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableFloat>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableFloat(a));
	}
}

void CSaveChunk::w_double(double a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableDouble>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableDouble(a));
	}
}

void CSaveChunk::w_vec3(const Fvector& a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableVec3>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableVec3(a));
	}
}

void CSaveChunk::w_vec4(const Fvector4& a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableVec4>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableVec4(a));
	}
}

void CSaveChunk::w_u64(u64 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableU64>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableU64(a));
	}
}

void CSaveChunk::w_s64(s64 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableS64>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableS64(a));
	}
}

void CSaveChunk::w_u32(u32 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableU32>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableU32(a));
	}
}

void CSaveChunk::w_s32(s32 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableS32>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableS32(a));
	}
}

void CSaveChunk::w_u16(u16 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableU16>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableU16(a));
	}
}

void CSaveChunk::w_s16(s16 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableS16>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableS16(a));
	}
}

void CSaveChunk::w_u8(u8 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableU8>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableU8(a));
	}
}

void CSaveChunk::w_s8(s8 a)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableS8>(a));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableS8(a));
	}
}

void CSaveChunk::w_float_q16(float a, float min, float max)
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
}

void CSaveChunk::w_stringZ(LPCSTR S)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableString>(S));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableString(S));
	}
}

void CSaveChunk::w_stringZ(const shared_str& p)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableString>(p));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableString(p));
	}
}

void CSaveChunk::w_stringZ(const xr_string& p)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableString>(p));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableString(p));
	}
}

void CSaveChunk::w_matrix(Fmatrix& M)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableMatrix>(M));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableMatrix(M));
	}
}

void CSaveChunk::w_clientID(ClientID& C)
{
	if (_currentArrayStack.empty()) {
		_variables.emplace_back(xr_make_unique<CSaveVariableClientID>(C));
	}
	else {
		_currentArrayStack.top()->AddVariable(new CSaveVariableClientID(C));
	}
}

void CSaveChunk::r_bool(bool& A)
{
	if (_currentArrayStack.empty()) {
		VERIFY(_variables[_currentReadIndex]->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<bool, CSaveVariableBool>(_variables[_currentReadIndex++].get());
	}
	else {
		auto CurrentArray = _currentArrayStack.top();
		VERIFY(CurrentArray->GetCurrentElement()->GetVariableType() == ESaveVariableType::t_bool);
		A = SSaveVariableGetter::GetValue<bool, CSaveVariableBool>((CSaveVariableBool*)CurrentArray->GetCurrentElement());
		CurrentArray->Next();
	}
}
