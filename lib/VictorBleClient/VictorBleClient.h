#ifndef VictorBleClient_h
#define VictorBleClient_h

#include <Arduino.h>
#include <BLEDevice.h>
#include <VictorBleClientCallbacks.h>
#include "VictorBleModels.h"

namespace Victor::Components {
  class VictorBleClient {
   public:
    VictorBleClient(BLEAdvertisedDevice* advertisedDevice);
    ~VictorBleClient();
    bool connectRemoteServer();
    bool isConnected();
    bool send(const String command);
    unsigned long lastHeartbeat = 0;
    typedef std::function<void(const VictorBleReport report)> TReportHandler;
    TReportHandler onReport = nullptr;

   private:
    BLEAdvertisedDevice* _advertisedDevice = nullptr;
    BLEClient* _client = nullptr;
    BLERemoteCharacteristic* _remoteCharacteristicReadable = nullptr;
    BLERemoteCharacteristic* _remoteCharacteristicWritable = nullptr;
    BLERemoteCharacteristic* _remoteCharacteristicNotifiable = nullptr;
    static VictorBleReport _parseReport(String str);
  };

} // namespace Victor::Components

#endif // VictorBleClient_h
