// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PLATFORM_IMPL_LINUX_WIFI_DIRECT_SERVER_SOCKET_H_
#define PLATFORM_IMPL_LINUX_WIFI_DIRECT_SERVER_SOCKET_H_

#include <sdbus-c++/IConnection.h>
#include "internal/platform/implementation/linux/network_manager_active_connection.h"
#include "internal/platform/implementation/linux/tcp_server_socket.h"
#include "internal/platform/implementation/linux/wifi_medium.h"
#include "internal/platform/implementation/wifi_direct.h"
namespace nearby {
namespace linux {
class NetworkManagerWifiDirectServerSocket
    : public api::WifiDirectServerSocket {
 public:
  NetworkManagerWifiDirectServerSocket(
      TCPServerSocket socket,
      std::unique_ptr<networkmanager::ActiveConnection> active_conn,
      std::shared_ptr<networkmanager::NetworkManager> network_manager)
      : server_socket_(std::move(socket)),
        active_conn_(std::move(active_conn)),
        network_manager_(std::move(network_manager)) {}

  std::string GetIPAddress() const override;
  int GetPort() const override;
  std::unique_ptr<api::WifiDirectSocket> Accept() override;
  Exception Close() override;

 private:
  TCPServerSocket server_socket_;
  std::unique_ptr<networkmanager::ActiveConnection> active_conn_;
  std::shared_ptr<networkmanager::NetworkManager> network_manager_;
};
}  // namespace linux
}  // namespace nearby

#endif
