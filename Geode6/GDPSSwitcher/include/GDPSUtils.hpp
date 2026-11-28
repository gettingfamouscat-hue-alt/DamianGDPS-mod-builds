#if 0
#ifndef GDPSUTILS_HPP
#define GDPSUTILS_HPP

#include "Types.hpp"
#include <string>
#include <Geode/loader/EventExport.hpp>
#define MY_MOD_ID "km7dev.gdps-switcher"

namespace GDPSUtils {
    /**
    * @brief Creates a new server with a specified name, url, and optionally a save directory.
    * 
    * @param name The name of the server.
    * @param url The URL of the server.
    * @param saveDir Save directory for the server, don't pass for default.
    * @return The id of the server.
    */
    GEODE_EVENT_EXPORT(geode::Result<int>, createServer, (std::string name, std::string url, std::string saveDir = ""), (name, url, saveDir));
    /**
    * @brief Retrieves a list of all available servers.
    * 
    * @return A map the available servers.
    */
    using ServerListResult = geode::Result<std::map<int, GDPSTypes::Server>>;
    GEODE_EVENT_EXPORT(ServerListResult, getServers, (), ());
    /**
    * @brief Retrieves the current server.
    *
    * @return The current server.
    */
    GEODE_EVENT_EXPORT(geode::Result<GDPSTypes::Server>, getCurrentServer, (), ());
    /**
    * @brief Sets the current server.
    *
    * @param id The id of the server to set as current.
    * @return True if the server was set successfully.
    */
    GEODE_EVENT_EXPORT(geode::Result<bool>, setCurrentServer, (int id), (id));
    /**
    * @brief Finds a server by its url and optionally its save directory.
    *
    * @param url The URL of the server to find.
    * @param saveDir The save directory of the server to find, don't pass to find any server with the same URL.
    * @return The server if found.
    */
    GEODE_EVENT_EXPORT(geode::Result<GDPSTypes::Server>, findServer, (std::string url, std::string saveDir = ""), (url, saveDir));
    /**
    * @brief Deletes a server.
    *
    * @param id The id of the server to delete.
    * @return True if the server was deleted successfully.
    */
    GEODE_EVENT_EXPORT(geode::Result<bool>, deleteServer, (int id), (id));
    /**
    * @brief Change the current server to the one with the specified id. Requires restart.
    *
    * @param id The id of the server to set as current.
    * @return True if the server was set successfully.
    */
    GEODE_EVENT_EXPORT(geode::Result<bool>, switchServer, (int id), (id));
    /**
    * @brief Retrieves information about a server by its id.
    *
    * @param id The id of the server to retrieve information about.
    * @return The server information if found.
    */
    GEODE_EVENT_EXPORT(geode::Result<GDPSTypes::Server>, getServerInfo, (int id), (id));
    /**
    * @brief Sets information for a server. Allows selective updates for name, URL, or save directory.
    *
    * @param id The id of the server to update.
    * @param name Optional new name of the server. Pass an empty string to keep the current name.
    * @param url Optional new URL of the server. Pass an empty string to keep the current URL.
    * @param saveDir Optional new save directory of the server. Pass an empty string to keep the current save directory.
    * @return True if the server information was updated successfully.
    */
    GEODE_EVENT_EXPORT(geode::Result<bool>, setServerInfo, (int id, std::string name = "", std::string url = "", std::string saveDir = ""), (id, name, url, saveDir));
};

#endif
#endif