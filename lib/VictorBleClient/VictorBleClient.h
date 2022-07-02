#ifndef VictorBleClient_h
#define VictorBleClient_h

#include <Arduino.h>
#include <BLEDevice.h>
#include <VictorBleClientCallbacks.h>

namespace Victor::Components {

  class VictorBleClient {
   public:
    VictorBleClient(BLEAdvertisedDevice* advertisedDevice);
    ~VictorBleClient();
    bool connectRemoteServer();
    bool heartbeat(const String message);

   private:
    BLEAdvertisedDevice* _advertisedDevice = nullptr;
    BLEClient* _client = nullptr;
    BLERemoteCharacteristic* _remoteCharacteristicReadable = nullptr;
    BLERemoteCharacteristic* _remoteCharacteristicWritable = nullptr;
    BLERemoteCharacteristic* _remoteCharacteristicNotifiable = nullptr;
  };

} // namespace Victor::Components

#endif // VictorBleClient_h
