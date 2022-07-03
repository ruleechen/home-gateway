#include "VictorBleClient.h"

namespace Victor::Components {

  VictorBleClient::VictorBleClient(BLEAdvertisedDevice* advertisedDevice) {
    _advertisedDevice = advertisedDevice;
  }

  VictorBleClient::~VictorBleClient() {
    if (_client != nullptr) {
      delete _client;
      _client = nullptr;
    }
    if (_remoteCharacteristicReadable != nullptr) {
      delete _remoteCharacteristicReadable;
      _remoteCharacteristicReadable = nullptr;
    }
    if (_remoteCharacteristicWritable != nullptr) {
      delete _remoteCharacteristicWritable;
      _remoteCharacteristicWritable = nullptr;
    }
    if (_remoteCharacteristicNotifiable != nullptr) {
      delete _remoteCharacteristicNotifiable;
      _remoteCharacteristicNotifiable = nullptr;
    }
  }

  bool VictorBleClient::connectRemoteServer() {
    _client = BLEDevice::createClient();
    const auto callbacks = new VictorBleClientCallbacks();
    callbacks->onConnectivityChange = [&](BLEClient* client, bool connected) {};
    _client->setClientCallbacks(callbacks);

    const auto success = _client->connect(_advertisedDevice);
    if (success) {
      const auto remoteServices = _client->getServices();
      for (auto service = remoteServices->begin(); service != remoteServices->end(); service++) {
        const auto remoteCharacteristics = service->second->getCharacteristics();
        for (auto characteristic = remoteCharacteristics->begin(); characteristic != remoteCharacteristics->end(); characteristic++) {
          const auto remoteCharacteristic = characteristic->second;
          if (remoteCharacteristic->canRead() && _remoteCharacteristicReadable == nullptr) {
            _remoteCharacteristicReadable = remoteCharacteristic;
            if (remoteCharacteristic->canNotify() && _remoteCharacteristicNotifiable == nullptr) {
              _remoteCharacteristicNotifiable = remoteCharacteristic;
            }
          }
          if (remoteCharacteristic->canWrite() && _remoteCharacteristicWritable == nullptr) {
            _remoteCharacteristicWritable = remoteCharacteristic;
          }
        }
      }
    }

    if (_remoteCharacteristicNotifiable != nullptr) {
      _remoteCharacteristicNotifiable->registerForNotify([](BLERemoteCharacteristic* remoteCharacteristic, uint8_t* data, size_t length, bool isNotify) {
        const auto dataStr = String(data, length);
        if (dataStr == "on0") {
          Serial.println("state is off");
        }
        if (dataStr == "on1") {
          Serial.println("state is on");
        }
      });
    }

    return success;
  }

  bool VictorBleClient::isConnected() {
    return _client->isConnected();
  }

  bool VictorBleClient::heartbeat(const String message) {
    if (_client->isConnected() && _remoteCharacteristicWritable != nullptr) {
      _remoteCharacteristicWritable->writeValue(message.c_str(), message.length());
      return true;
    }
    return false;
  }

} // namespace Victor::Components
