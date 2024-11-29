#include "stdafx.h"
#include "script_export_space.h"

SCRIPTS_API xr_hash_map<script_exporter_key_base, script_exporter_data*>& get_script_export_container()
{
	static xr_hash_map<script_exporter_key_base, script_exporter_data*> script_exporter_container;
	return script_exporter_container;
}
