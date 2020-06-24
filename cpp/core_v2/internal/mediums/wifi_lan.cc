#include "core_v2/internal/mediums/wifi_lan.h"

#include <memory>
#include <string>
#include <utility>

#include "platform_v2/public/logging.h"
#include "platform_v2/public/mutex_lock.h"

namespace location {
namespace nearby {
namespace connections {

bool WifiLan::IsAvailable() const {
  MutexLock lock(&mutex_);

  return IsAvailableLocked();
}

bool WifiLan::IsAvailableLocked() const { return medium_.IsValid(); }

bool WifiLan::StartAdvertising(const std::string& service_id,
                               const std::string& wifi_lan_service_info_name) {
  MutexLock lock(&mutex_);

  if (wifi_lan_service_info_name.empty()) {
    NEARBY_LOG(
        INFO,
        "Refusing to turn on WifiLan advertising. Empty service info name.");
    return false;
  }

  if (!IsAvailableLocked()) {
    NEARBY_LOG(INFO,
               "Can't turn on WifiLan advertising. WifiLan is not available.");
    return false;
  }

  if (!medium_.StartAdvertising(service_id, wifi_lan_service_info_name)) {
    NEARBY_LOG(
        INFO, "Failed to turn on WifiLan advertising with service info name=%s",
        wifi_lan_service_info_name.c_str());
    return false;
  }

  NEARBY_LOG(INFO, "Turned on WifiLan advertising with service info name=%s",
             wifi_lan_service_info_name.c_str());
  advertising_info_.service_id = service_id;
  return true;
}

void WifiLan::StopAdvertising(const std::string& service_id) {
  MutexLock lock(&mutex_);

  if (!IsAdvertisingLocked()) {
    NEARBY_LOG(INFO, "Can't turn off WifiLan advertising; it is already off");
    return;
  }

  medium_.StopAdvertising(advertising_info_.service_id);
  // Reset our bundle of advertising state to mark that we're no longer
  // advertising.
  advertising_info_.Clear();
}

bool WifiLan::IsAdvertising() {
  MutexLock lock(&mutex_);

  return IsAdvertisingLocked();
}

bool WifiLan::IsAdvertisingLocked() {
  return !advertising_info_.Empty();
}

bool WifiLan::StartDiscovery(const std::string& service_id,
                             DiscoveredServiceCallback callback) {
  MutexLock lock(&mutex_);

  if (service_id.empty()) {
    NEARBY_LOG(INFO,
               "Refusing to start WifiLan discovering with empty service id.");
    return false;
  }

  if (!IsAvailableLocked()) {
    NEARBY_LOG(
        INFO,
        "Can't discover WifiLan services because WifiLan isn't available.");
    return false;
  }

  if (IsDiscoveringLocked(service_id)) {
    NEARBY_LOG(
        INFO,
        "Refusing to start discovery of WifiLan services because another "
        "discovery is already in-progress.");
    return false;
  }

  if (!medium_.StartDiscovery(service_id, callback)) {
    NEARBY_LOG(INFO, "Failed to start discovery of WifiLan services.");
    return false;
  }

  // Mark the fact that we're currently performing a WifiLan discovering.
  discovering_info_.service_id = service_id;
  return true;
}

void WifiLan::StopDiscovery(const std::string& service_id) {
  MutexLock lock(&mutex_);

  if (!IsDiscoveringLocked(service_id)) {
    NEARBY_LOG(INFO,
               "Can't turn off WifiLan discovering because we never started "
               "discovering.");
    return;
  }

  medium_.StopDiscovery(service_id);
  discovering_info_.Clear();
}

bool WifiLan::IsDiscovering(const std::string& service_id) {
  MutexLock lock(&mutex_);

  return IsDiscoveringLocked(service_id);
}

bool WifiLan::IsDiscoveringLocked(const std::string& service_id) {
  return !discovering_info_.Empty();
}

bool WifiLan::StartAcceptingConnections(const std::string& service_id,
                                        AcceptedConnectionCallback callback) {
  MutexLock lock(&mutex_);

  if (service_id.empty()) {
    NEARBY_LOG(INFO,
               "Refusing to start accepting WifiLan connections with empty "
               "service id.");
    return false;
  }

  if (!IsAvailableLocked()) {
    NEARBY_LOG(INFO,
               "Can't start accepting WifiLan connections for %s because "
               "WifiLan isn't available.",
               service_id.c_str());
    return false;
  }

  if (IsAcceptingConnectionsLocked(service_id)) {
    NEARBY_LOG(INFO,
               "Refusing to start accepting WifiLan connections for %s because "
               "another WifiLan service socket is already in-progress.",
               service_id.c_str());
    return false;
  }

  if (!medium_.StartAcceptingConnections(service_id, callback)) {
    NEARBY_LOG(INFO, "Failed to accept connections callback for %s.",
               service_id.c_str());
    return false;
  }

  accepting_connections_info_.service_id = service_id;
  return true;
}

void WifiLan::StopAcceptingConnections(const std::string& service_id) {
  MutexLock lock(&mutex_);

  if (!IsAcceptingConnectionsLocked(service_id)) {
    NEARBY_LOG(INFO,
               "Can't stop accepting WifiLan connections because it was never "
               "started.");
    return;
  }

  medium_.StopAcceptingConnections(accepting_connections_info_.service_id);
  // Reset our bundle of accepting connections state to mark that we're no
  // longer accepting connections.
  accepting_connections_info_.Clear();
}

bool WifiLan::IsAcceptingConnections(const std::string& service_id) {
  MutexLock lock(&mutex_);

  return IsAcceptingConnectionsLocked(service_id);
}

bool WifiLan::IsAcceptingConnectionsLocked(const std::string& service_id) {
  return !accepting_connections_info_.Empty();
}

WifiLanSocket WifiLan::Connect(WifiLanService& wifi_lan_service,
                               const std::string& service_id) {
  MutexLock lock(&mutex_);
  NEARBY_LOG(INFO, "WifiLan::Connect: service=%p", &wifi_lan_service);
  // Socket to return. To allow for NRVO to work, it has to be a single object.
  WifiLanSocket socket;

  if (service_id.empty()) {
    NEARBY_LOG(INFO,
               "Refusing to create WifiLan socket with empty service_id.");
    return socket;
  }

  if (!IsAvailableLocked()) {
    NEARBY_LOG(INFO,
               "Can't create client WifiLan socket [service_id=%s]; WifiLan "
               "isn't available.",
               service_id.c_str());
    return socket;
  }

  socket = medium_.Connect(wifi_lan_service, service_id);
  if (!socket.IsValid()) {
    NEARBY_LOG(INFO, "Failed to Connect via WifiLan [service=%s]",
               service_id.c_str());
  }

  return socket;
}

}  // namespace connections
}  // namespace nearby
}  // namespace location
