#pragma once
#include "StdAfx.h"
#include "ShapeData.h"
#include "Save/SaveObject.h"

ISaveObject& operator<<(ISaveObject& Object, CShapeData::shape_def& Value) {
	Object << Value.type;
	switch (Value.type) {
	case CShapeData::cfSphere: {
		Object << Value.data.sphere;
		break;
	}
	case CShapeData::cfBox: {
		Object << Value.data.box;
		break;
	}
	}
	return Object;
}
