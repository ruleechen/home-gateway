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
    if (onNotify != nullptr) {
      onNotify = nullptr;
    }
  }

  bool VictorBleClient::connectServer() {
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
        const auto notification = _parseNotification(dataStr);
        if (notification->type == SERVER_NOTIFY_HEARTBEAT) {
          lastHeartbeat = millis();
        }
        if (onNotify != nullptr) {
          onNotify(notification);
        }
      });
    }

    return success;
  }

  bool VictorBleClient::isConnected() {
    return _client->isConnected();
  }

  bool VictorBleClient::send(const ServerCommand command) {
    auto sent = false;
    if (_client->isConnected() && _remoteCharacteristicWritable != nullptr) {
      const auto message = command.toStr();
      _remoteCharacteristicWritable->writeValue(message.c_str(), message.length());
      sent = true;
    }
    return sent;
  }

  ServerNotification* VictorBleClient::_parseNotification(String str) {
    auto code = str.substring(0, 2);
    auto args = str.substring(3);
    auto type = SERVER_NOTIFY_NONE;
    if (code == "HB") {
      type = SERVER_NOTIFY_HEARTBEAT;
    } else if (code == "PW") {
      type = SERVER_NOTIFY_POWER;
    } else if (code == "ON") {
      type = SERVER_NOTIFY_ON;
    }
    if (type == SERVER_NOTIFY_NONE) {
      return nullptr;
    }
    auto next = _parseNotification(args);
    ServerNotification notification = {
      type = type,
      args = args,
      next  = next,
    };
    const auto pNotify = &notification;
    return pNotify;
  }

} // namespace Victor::Components
