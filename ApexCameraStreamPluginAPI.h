/**********************************************************\

  Auto-generated ApexCameraStreamPluginAPI.h

  \**********************************************************/

#include <string>
#include <sstream>
#include <stdbool.h>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"

#include "ApexCameraStreamPlugin.h"

#ifndef H_ApexCameraStreamPluginAPI
#define H_ApexCameraStreamPluginAPI

class ApexCameraStreamPluginAPI : public FB::JSAPIAuto
{
public:
	////////////////////////////////////////////////////////////////////////////
	/// @fn ApexCameraStreamPluginAPI::ApexCameraStreamPluginAPI(const ApexCameraStreamPluginPtr& plugin, const FB::BrowserHostPtr host)
	///
	/// @brief  Constructor for your JSAPI object.
	///         You should register your methods, properties, and events
	///         that should be accessible to Javascript from here.
	///
	/// @see FB::JSAPIAuto::registerMethod
	/// @see FB::JSAPIAuto::registerProperty
	/// @see FB::JSAPIAuto::registerEvent
	////////////////////////////////////////////////////////////////////////////
	ApexCameraStreamPluginAPI(const ApexCameraStreamPluginPtr& plugin, const FB::BrowserHostPtr& host) :
		m_plugin(plugin), m_host(host)
	{
		registerMethod("connect", make_method(this, &ApexCameraStreamPluginAPI::connectToServer));
		registerMethod("disconnect", make_method(this, &ApexCameraStreamPluginAPI::disconnect));

		// Read-only property
		registerProperty("version",
			make_property(this,
			&ApexCameraStreamPluginAPI::get_version));

		// Read-only property
		registerProperty("status",
			make_property(this, &ApexCameraStreamPluginAPI::get_status));
	}

	///////////////////////////////////////////////////////////////////////////////
	/// @fn ApexCameraStreamPluginAPI::~ApexCameraStreamPluginAPI()
	///
	/// @brief  Destructor.  Remember that this object will not be released until
	///         the browser is done with it; this will almost definitely be after
	///         the plugin is released.
	///////////////////////////////////////////////////////////////////////////////
	virtual ~ApexCameraStreamPluginAPI() {};

	ApexCameraStreamPluginPtr getPlugin();

	// Read-only property ${PROPERTY.ident}
	std::string get_version();

	// Read-only property status
	std::string get_status();

	// Event helpers
	FB_JSAPI_EVENT(statuschanged, 1, (const std::string&));

	// methods to connect and disconnect
	void connectToServer(const std::string);
	void disconnect();

private:
	ApexCameraStreamPluginWeakPtr m_plugin;
	FB::BrowserHostPtr m_host;
};

#endif // H_ApexCameraStreamPluginAPI

