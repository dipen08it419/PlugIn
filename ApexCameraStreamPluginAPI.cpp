/**********************************************************\

  Auto-generated ApexCameraStreamPluginAPI.cpp

  \**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"

#include "ApexCameraStreamPluginAPI.h"

ApexCameraStreamPluginPtr ApexCameraStreamPluginAPI::getPlugin()
{
	ApexCameraStreamPluginPtr plugin(m_plugin.lock());
	if (!plugin) {
		throw FB::script_error("The plugin is invalid");
	}
	return plugin;
}

// Read-only property status
std::string ApexCameraStreamPluginAPI::get_status()
{
	return getPlugin()->getStatus();
}

// Read-only property version
std::string ApexCameraStreamPluginAPI::get_version()
{
	return FBSTRING_PLUGIN_VERSION;
}

void ApexCameraStreamPluginAPI::connectToServer(std::string streamUrl)
{
	getPlugin()->connectToServer(streamUrl);
}

void ApexCameraStreamPluginAPI::disconnect()
{
	getPlugin()->disconnect();
}