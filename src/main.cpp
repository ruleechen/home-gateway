#include <map>
#include <vector>
#include <Arduino.h>
#include <BLEDevice.h>
#include <VictorBleClient.h>

using namespace Victor;
using namespace Victor::Components;

/* Specify the Service UUID of Server */
static BLEUUID advertisingServiceUUID("00001002-0000-1000-8000-00805f9b34fb");
static BLEScan* scan = nullptr;

static std::vector<std::string> advertisedAddresses = {};
static std::map<std::string, VictorBleClient*> clients = {};

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.printf("BLE Advertised Device found [%s]", advertisedDevice.toString().c_str()); Serial.println();
    if (
      advertisedDevice.haveServiceUUID() &&
      advertisedDevice.isAdvertisingService(advertisingServiceUUID)
    ) {
      const auto address = advertisedDevice.getAddress().toString();
      if (clients.count(address) == 0) {
        const auto client = new VictorBleClient(new BLEAdvertisedDevice(advertisedDevice));
        client->onNotify = [](BLEAddress& serverAddress, ServerNotification* notification) {
          auto item = notification;
          while (item != nullptr && item->type) {
            Serial.printf("[%s] ", serverAddress.toString().c_str());
            Serial.println(item->raw);
            item = item->next;
          }
        };
        advertisedAddresses.push_back(address);
        clients[address] = client;
        Serial.printf("Created client for server [%s]", address.c_str()); Serial.println();
      }
    }
  }
};

static unsigned long lastLoop = 0;
static unsigned long lastHeartbeat = 0;

void setup() {
  Serial.begin(115200);
  BLEDevice::init("Victor-Gateway");
  // init scan
  scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  scan->setInterval(1349);
  scan->setWindow(449);
  scan->setActiveScan(true);
  scan->start(5, true);
}

static bool alarmOnState = false;

void loop() {
  const auto now = millis();
  if (now - lastLoop > 5000) {
    lastLoop = now;
    if (advertisedAddresses.size() > 0) {
      for (auto address : advertisedAddresses) {
        const auto client = clients[address];
        client->connectServer();
        Serial.printf("Connecting server [%s]", address.c_str()); Serial.println();
      }
      advertisedAddresses.clear();
    }

    std::vector<std::string> disconnectedAddresses = {};
    for (auto it = clients.begin(); it != clients.end(); ++it) {
      const auto client = clients[it->first];
      if (!client->isConnected()) {
        disconnectedAddresses.push_back(it->first);
      } else {
        client->send({
          .type = SERVER_COMMAND_SET_ALARM,
          .args = alarmOnState ? "1" : "0",
        });
        alarmOnState = !alarmOnState;
      }
    }
    for (auto address : disconnectedAddresses) {
      clients.erase(address);
      Serial.printf("Removed client [%s]", address.c_str()); Serial.println();
    }
    Serial.printf("Clients [%d]", clients.size()); Serial.println();
  }
}
