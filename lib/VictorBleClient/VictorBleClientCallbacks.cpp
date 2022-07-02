#include "VictorBleClientCallbacks.h"

namespace Victor::Components {

  VictorBleClientCallbacks::~VictorBleClientCallbacks() {
    onConnectivityChange = nullptr;
  }

  void VictorBleClientCallbacks::onConnect(BLEClient* client) {
    if (onConnectivityChange != nullptr) {
      onConnectivityChange(client, true);
    }
  }

  void VictorBleClientCallbacks::onDisconnect(BLEClient* client) {
    if (onConnectivityChange != nullptr) {
      onConnectivityChange(client, false);
    }
  }

} // namespace Victor::Components
