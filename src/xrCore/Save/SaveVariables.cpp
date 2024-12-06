#include "stdafx.h"
#include "SaveVariables.h"

CSaveVariableArray::~CSaveVariableArray()
{
	for (size_t i = 0; i < _array.size(); ++i) {
		xr_delete(_array[i]);
	}
}
