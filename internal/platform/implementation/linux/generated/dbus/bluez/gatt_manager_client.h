
/*
 * This file was automatically generated by sdbus-c++-xml2cpp; DO NOT EDIT!
 */

#ifndef __sdbuscpp__gatt_manager_client_h__proxy__H__
#define __sdbuscpp__gatt_manager_client_h__proxy__H__

#include <sdbus-c++/sdbus-c++.h>
#include <string>
#include <tuple>

namespace org {
namespace bluez {

class GattManager1_proxy
{
public:
    static constexpr const char* INTERFACE_NAME = "org.bluez.GattManager1";

protected:
    GattManager1_proxy(sdbus::IProxy& proxy)
        : proxy_(proxy)
    {
    }

    ~GattManager1_proxy() = default;

public:
    void RegisterApplication(const sdbus::ObjectPath& application, const std::map<std::string, sdbus::Variant>& options)
    {
        proxy_.callMethod("RegisterApplication").onInterface(INTERFACE_NAME).withArguments(application, options);
    }

    void UnregisterApplication(const sdbus::ObjectPath& application)
    {
        proxy_.callMethod("UnregisterApplication").onInterface(INTERFACE_NAME).withArguments(application);
    }

private:
    sdbus::IProxy& proxy_;
};

}} // namespaces

#endif
