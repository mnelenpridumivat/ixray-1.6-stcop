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
		Fsphere		sphere = {};
		Fmatrix		box;
	};
	struct shape_def
	{
		u8			type;
		shape_data	data;

		friend void to_json(nlohmann::json& file, const shape_def& value)
		{
			file["type"] = (u64)value.type;
			switch (value.type)
			{
				case cfSphere:
					{
						file["data"] = value.data.sphere;
						break;
					}
				case cfBox:
					{
						file["data"] = value.data.box;
						break;
					}
			}
		}
	
		friend void from_json(const nlohmann::json& file, shape_def& value)
		{
			value.type = file["type"];
			switch (value.type)
			{
			case cfSphere:
				{
					value.data.sphere = file["data"];
					break;
				}
			case cfBox:
				{
					value.data.box = file["data"];
					break;
				}
			}
		}
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
