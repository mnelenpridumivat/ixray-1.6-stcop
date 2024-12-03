#pragma once

#include "alife_space.h"
#include "object_interfaces.h"

//using INFO_DATA = shared_str;

struct INFO_DATA : public IPureSerializeObject<IReader, IWriter>
{
	INFO_DATA() :info_id(nullptr), receive_time(0) {};
	INFO_DATA(shared_str id, ALife::_TIME_ID time) :info_id(id), receive_time(time) {};

	void		load(IReader& stream) override;
	void		save(IWriter&) override;

	shared_str			info_id;
	//время получения нужно порции информации
	ALife::_TIME_ID		receive_time;
};

using KNOWN_INFO_VECTOR = xr_vector<INFO_DATA>;
using KNOWN_INFO_VECTOR_IT = KNOWN_INFO_VECTOR::iterator;

class CFindByIDPred
{
public:
	CFindByIDPred(shared_str element_to_find) {element = element_to_find;}
	IC bool operator () (const INFO_DATA& data) const {return data.info_id == element;}
private:
	shared_str element;
};
