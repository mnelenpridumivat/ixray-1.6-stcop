#include "stdafx.h"
#include "script_export_space.h"

SCRIPTS_API script_export_hashmap& get_script_export_container()
{
	static script_export_hashmap script_exporter_container;
	return script_exporter_container;
}

SCRIPTS_API script_export_hashmap& get_script_export_container_xrSE_Factory()
{
	static script_export_hashmap script_exporter_container;
	return script_exporter_container;
}

SCRIPTS_API script_export_hashmap& get_script_export_container_xrGame()
{
	static script_export_hashmap script_exporter_container;
	return script_exporter_container;
}