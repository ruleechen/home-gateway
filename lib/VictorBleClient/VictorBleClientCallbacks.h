#ifndef VictorBleClientCallbacks_h
#define VictorBleClientCallbacks_h

#include <functional>
#include <BLEDevice.h>

namespace Victor::Components {
  class VictorBleClientCallbacks : public BLEClientCallbacks {
   public:
    ~VictorBleClientCallbacks();
    typedef std::function<void(BLEClient* client, bool connected)> TConnectivityHandler;
    TConnectivityHandler onConnectivityChange = nullptr;
    void onConnect(BLEClient* client) override;
    void onDisconnect(BLEClient* client) override;
  };
} // namespace Victor::Components

#endif // VictorBleClientCallbacks_h
