#ifndef PLATFORM_IMPL_LINUX_BLUEZ_H_
#define PLATFORM_IMPL_LINUX_BLUEZ_H_

#include <sdbus-c++/ProxyInterfaces.h>
#include <sdbus-c++/StandardInterfaces.h>
#include <sdbus-c++/Types.h>

#include "absl/strings/string_view.h"

#include <string>

#define BLUEZ_LOG_METHOD_CALL_ERROR(proxy, method, err)                        \
  do {                                                                         \
    NEARBY_LOGS(ERROR) << __func__ << ": Got error '" << (err).getName()       \
                       << "' with message '" << (err).getMessage()             \
                       << "' while calling " << method << " on object "        \
                       << (proxy)->getObjectPath();                            \
  } while (false)

namespace nearby {
namespace linux {
namespace bluez {
static constexpr const char *SERVICE_DEST = "org.bluez";

static constexpr const char *ADAPTER_INTERFACE = "org.bluez.Adapter1";

static constexpr const char *DEVICE_INTERFACE = "org.bluez.Device1";
static constexpr const char *DEVICE_PROP_ADDRESS = "Address";
static constexpr const char *DEVICE_PROP_ALIAS = "Alias";
static constexpr const char *DEVICE_PROP_PAIRED = "Paired";
static constexpr const char *DEVICE_PROP_CONNECTED = "Connected";

 std::string
device_object_path(const sdbus::ObjectPath &adapter_object_path,
                   absl::string_view mac_address);

 sdbus::ObjectPath profile_object_path(absl::string_view service_uuid);

 sdbus::ObjectPath adapter_object_path(absl::string_view name);

class BluezObjectManager
    : public sdbus::ProxyInterfaces<sdbus::ObjectManager_proxy> {
public:
  BluezObjectManager(sdbus::IConnection &system_bus)
      : ProxyInterfaces(system_bus, "org.bluez", "/") {
    registerProxy();
  }
  ~BluezObjectManager() { unregisterProxy(); }

protected:
  void onInterfacesAdded(
      const sdbus::ObjectPath &objectPath,
      const std::map<std::string, std::map<std::string, sdbus::Variant>>
          &interfacesAndProperties) override {}
  void
  onInterfacesRemoved(const sdbus::ObjectPath &objectPath,
                      const std::vector<std::string> &interfaces) override {}
};

} // namespace bluez
} // namespace linux
} // namespace nearby

#endif
