#ifndef VictorBleClient_h
#define VictorBleClient_h

#include <functional>
#include <Arduino.h>
#include <BLEDevice.h>
#include "VictorBleModels.h"

#ifndef VICTOR_BLE_HEARTBEAT_INTERVAL
#define VICTOR_BLE_HEARTBEAT_INTERVAL 10 * 60 * 1000
#endif

#ifndef VICTOR_BLE_AUTHENTICATE_TOKEN
#define VICTOR_BLE_AUTHENTICATE_TOKEN "RS20220718"
#endif

namespace Victor::Components {
  class VictorBleClientCallbacks : public BLEClientCallbacks {
   public:
    typedef std::function<void(BLEClient* client, bool connected)> TConnectivityHandler;
    TConnectivityHandler onConnectivityChange = nullptr;
    ~VictorBleClientCallbacks() {
      onConnectivityChange = nullptr;
    }
    void onConnect(BLEClient* client) {
      if (onConnectivityChange != nullptr) {
        onConnectivityChange(client, true);
      }
    }
    void onDisconnect(BLEClient* client) {
      if (onConnectivityChange != nullptr) {
        onConnectivityChange(client, false);
      }
    }
  };

  class VictorBleClient {
   public:
    VictorBleClient(BLEAdvertisedDevice* advertisedDevice);
    ~VictorBleClient();
    void loop();
    bool connectServer();
    bool isConnected();
    bool send(const String message);
    bool send(const ServerCommand command);
    static ServerNotifyType parseNotifyType(const String& str);
    static ServerNotification* parseNotification(const String& str);
    typedef std::function<void(BLEAddress& serverAddress, ServerNotification* notification)> TNotifyHandler;
    TNotifyHandler onNotify = nullptr;

   private:
    BLEAdvertisedDevice* _advertisedDevice = nullptr;
    BLEClient* _client = nullptr;
    unsigned long _lastHeartbeat = 0;
    BLERemoteCharacteristic* _remoteCharacteristicReadable = nullptr;
    BLERemoteCharacteristic* _remoteCharacteristicWritable = nullptr;
    BLERemoteCharacteristic* _remoteCharacteristicNotifiable = nullptr;
  };

} // namespace Victor::Components

#endif // VictorBleClient_h
