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

  void VictorBleClient::loop() {
    // heartbeat every 5 minutes
    const auto now = millis();
    if (now - _lastHeartbeat > VICTOR_BLE_HEARTBEAT_INTERVAL) {
      _lastHeartbeat = now;
      send({
        .type = SERVER_COMMAND_HEARTBEAT,
        .args = VICTOR_BLE_AUTHENTICATE_TOKEN,
      });
    }
  }

  bool VictorBleClient::connectServer() {
    _client = BLEDevice::createClient();
    const auto callbacks = new VictorBleClientCallbacks();
    callbacks->onConnectivityChange = [&](BLEClient* client, bool connected) {
      const auto address = _advertisedDevice->getAddress().toString().c_str();
      Serial.printf("[%s] %s", address, connected ? "connected" : "disconnected"); Serial.println();
    };
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
      _remoteCharacteristicNotifiable->registerForNotify([&](BLERemoteCharacteristic* remoteCharacteristic, uint8_t* data, size_t length, bool isNotify) {
        const auto dataStr = String(data, length);
        const auto notification = parseNotification(dataStr);
        if (notification == nullptr) {
          const auto address = _advertisedDevice->getAddress().toString().c_str();
          Serial.printf("[%s] %s", address, dataStr); Serial.println();
          return;
        }
        if (
          notification->type == SERVER_NOTIFY_READY ||
          notification->type == SERVER_NOTIFY_HEARTBEAT
        ) {
          _lastHeartbeat = millis();
        }
        if (onNotify != nullptr) {
          auto serverAddress = _advertisedDevice->getAddress();
          onNotify(serverAddress, notification);
        }
      });

      send({
        .type = SERVER_COMMAND_AUTHENTICATE,
        .args = VICTOR_BLE_AUTHENTICATE_TOKEN,
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

  bool VictorBleClient::send(const ServerCommand command) {
    const auto message = command.serialize();
    return send(message);
  }

  ServerNotifyType VictorBleClient::parseNotifyType(const String& code) {
    if (code == "RDY") {
      return SERVER_NOTIFY_READY;
    } else if (code == "HRB") {
      return SERVER_NOTIFY_HEARTBEAT;
    } else if (code == "BTY") {
      return SERVER_NOTIFY_BATTERY;
    } else if (code == "ON") {
      return SERVER_NOTIFY_ON;
    }
    return SERVER_NOTIFY_NONE;
  }

  ServerNotification* VictorBleClient::parseNotification(const String& str) {
    // split
    const auto index = str.indexOf(':');
    if (index < 1) {
      return nullptr;
    }
    const auto code = str.substring(0, index);
    const auto args = str.substring(index + 1);
    // type
    const auto type = parseNotifyType(code);
    if (type == SERVER_NOTIFY_NONE) {
      return nullptr;
    }
    // struct
    const auto next = parseNotification(args);
    const auto notification = new ServerNotification({
      .type = type,
      .args = args,
      .raw  = str,
      .next = next,
    });
    return notification;
  }

} // namespace Victor::Components
