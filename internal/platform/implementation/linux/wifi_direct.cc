#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <sys/socket.h>

#include "internal/platform/implementation/linux/wifi_direct.h"
#include "internal/platform/implementation/linux/wifi_direct_server_socket.h"
#include "internal/platform/implementation/linux/wifi_direct_socket.h"
#include "internal/platform/implementation/linux/wifi_hotspot.h"
#include "internal/platform/implementation/linux/wifi_medium.h"
#include "internal/platform/implementation/wifi_direct.h"
#include "internal/platform/wifi_credential.h"

namespace nearby {
namespace linux {
std::unique_ptr<api::WifiDirectSocket>
NetworkManagerWifiDirectMedium::ConnectToService(
    absl::string_view ip_address, int port,
    CancellationFlag *cancellation_flag) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    NEARBY_LOGS(ERROR) << __func__
                       << ": Error opening socket: " << std::strerror(errno);
    return nullptr;
  }

  NEARBY_LOGS(VERBOSE) << __func__ << ": Connecting to " << ip_address << ":"
                       << port;
  struct sockaddr_in addr;
  addr.sin_addr.s_addr = inet_addr(std::string(ip_address).c_str());
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  auto ret =
      connect(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
  if (ret < 0) {
    NEARBY_LOGS(ERROR) << __func__ << ": Error connecting to socket: "
                       << std::strerror(errno);
    return nullptr;
  }

  return std::make_unique<WifiDirectSocket>(sock);
}

std::unique_ptr<api::WifiDirectServerSocket>
NetworkManagerWifiDirectMedium::ListenForService(int port) {
  auto active_connection = wireless_device_->GetActiveConnection();
  if (active_connection == nullptr) {
    return nullptr;
  }

  auto ip4addresses = active_connection->GetIP4Addresses();
  if (ip4addresses.empty()) {
    NEARBY_LOGS(ERROR)
        << __func__
        << "Could not find any IPv4 addresses for active connection "
        << active_connection->getObjectPath();
    return nullptr;
  }

  auto sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    NEARBY_LOGS(ERROR) << __func__
                       << ": Error opening socket: " << std::strerror(errno);
    return nullptr;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip4addresses[0].c_str());
  addr.sin_port = htons(port);

  auto ret =
      bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
  if (ret < 0) {
    NEARBY_LOGS(ERROR) << __func__
                       << ": Error binding to socket: " << std::strerror(errno);
    return nullptr;
  }

  NEARBY_LOGS(VERBOSE) << __func__ << ": Listening for services on "
                       << ip4addresses[0] << ":" << port << " on device "
                       << wireless_device_->getObjectPath();

  ret = listen(sock, 0);
  if (ret < 0) {
    NEARBY_LOGS(ERROR) << __func__ << ": Error listening on socket: "
                       << std::strerror(errno);
    return nullptr;
  }

  return std::make_unique<NetworkManagerWifiDirectServerSocket>(
      sock, system_bus_, active_connection->getObjectPath(), network_manager_);
}

bool NetworkManagerWifiDirectMedium::ConnectWifiDirect(
    WifiDirectCredentials *wifi_direct_credentials) {
  if (wifi_direct_credentials == nullptr) {
    NEARBY_LOGS(ERROR) << __func__ << ": hotspot_credentials cannot be null";
    return false;
  }

  auto ssid = wifi_direct_credentials->GetSSID();
  auto password = wifi_direct_credentials->GetPassword();

  return wireless_device_->ConnectToNetwork(ssid, password,
                                            api::WifiAuthType::kWpaPsk) ==
         api::WifiConnectionStatus::kConnected;
}

bool NetworkManagerWifiDirectMedium::DisconnectWifiDirect() {
  if (!ConnectedToWifi()) {
    NEARBY_LOGS(ERROR) << __func__ << ": Not connected to a WiFi hotspot";
    return false;
  }

  auto active_connection = wireless_device_->GetActiveConnection();
  if (active_connection == nullptr) {
    return false;
  }

  try {
    network_manager_->DeactivateConnection(active_connection->getObjectPath());
  } catch (const sdbus::Error &e) {
    DBUS_LOG_METHOD_CALL_ERROR(network_manager_, "DeactivateConnection", e);
    return false;
  }

  return true;
}

bool NetworkManagerWifiDirectMedium::StartWifiDirect(
    WifiDirectCredentials *wifi_direct_credentials) {
  // According to the comments in the windows implementation, the wifi direct
  // medium is currently just a regular wifi hotspot.  
  auto wireless_device = std::make_unique<NetworkManagerWifiMedium>(
      network_manager_, system_bus_, wireless_device_->getObjectPath());
  auto hotspot = NetworkManagerWifiHotspotMedium(system_bus_, network_manager_,
                                                 std::move(wireless_device));

  HotspotCredentials hotspot_creds;
  if (!hotspot.StartWifiHotspot(&hotspot_creds))
    return false;

  wifi_direct_credentials->SetSSID(hotspot_creds.GetSSID());
  wifi_direct_credentials->SetPassword(hotspot_creds.GetPassword());
  return true;
}

bool NetworkManagerWifiDirectMedium::StopWifiDirect() {
  auto wireless_device = std::make_unique<NetworkManagerWifiMedium>(
      network_manager_, system_bus_, wireless_device_->getObjectPath());
  auto hotspot = NetworkManagerWifiHotspotMedium(system_bus_, network_manager_,
                                                 std::move(wireless_device));

  return hotspot.DisconnectWifiHotspot();
}

} // namespace linux
} // namespace nearby
