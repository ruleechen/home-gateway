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
    if (onReport != nullptr) {
      onReport = nullptr;
    }
  }

  bool VictorBleClient::connectRemoteServer() {
    _client = BLEDevice::createClient();
    const auto callbacks = new VictorBleClientCallbacks();
    callbacks->onConnectivityChange = [&](BLEClient* client, bool connected) {
      Serial.println(connected ? "Connected" : "Disconnected");
    };
    _client->setClientCallbacks(callbacks);

    const auto success = _client->connect(_advertisedDevice);
    if (success) {
      lastHeartbeat = millis(); // connect success means it is alive
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
      _remoteCharacteristicNotifiable->registerForNotify([&](BLERemoteCharacteristic* remoteCharacteristic, uint8_t* data, size_t length, bool isNotify) {
        const auto dataStr = String(data, length);
        const auto report = _parseReport(dataStr);
        if (report.type == BLE_REPORT_TYPE_HEARTBEAT) {
          lastHeartbeat = millis();
        }
        if (onReport != nullptr) {
          onReport(report);
        }
      });
    }

    return success;
  }

  bool VictorBleClient::isConnected() {
    return _client->isConnected();
  }

  bool VictorBleClient::send(const String message) {
    auto sent = false;
    if (_client->isConnected() && _remoteCharacteristicWritable != nullptr) {
      _remoteCharacteristicWritable->writeValue(message.c_str(), message.length());
      sent = true;
    }
    return sent;
  }

  VictorBleReport VictorBleClient::_parseReport(String str) {
    const auto type  = str.substring(0, 2);
    const auto value = str.substring(3);
    VictorBleReport report = {};
    report.rawReport = str;
    if (type == "HB") {
      report.type = BLE_REPORT_TYPE_HEARTBEAT;
    } else if (type == "PW") {
      report.type = BLE_REPORT_TYPE_POWER;
    } else if (type == "ST") {
      report.type = BLE_REPORT_TYPE_STATE;
    }
    report.value = value;
    return report;
  }

} // namespace Victor::Components
