#pragma once
#include "Save/SaveInterface.h"

struct XRCORE_API CShapeData
{
	enum{
    	cfSphere=0,
        cfBox
    };
	union shape_data
	{
		Fsphere		sphere;
		Fmatrix		box;
	};
	struct shape_def
	{
		u8			type;
		shape_data	data;
	};

	using ShapeVec = xr_vector<shape_def>;
	using ShapeIt = ShapeVec::iterator;

	ShapeVec						shapes;
};

XRCORE_API ISaveObject& operator<<(ISaveObject& Object, CShapeData::shape_def& Value); /* {
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
}*/
