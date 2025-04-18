#pragma once
#include "xrstring.h"
#include <variant>
#include "xr_ini.h"

class CInifile;

namespace Logic
{

	class Template {

		struct Param {
			shared_str Key;
			xr_vector<std::variant<xr_string, size_t>> Value;
		};

		xr_vector<shared_str> TemplateParams;
		xr_vector<Param> Params;

	public:
		Template(CInifile::Sect* Ltx);

	};
}