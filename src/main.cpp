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
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (
      advertisedDevice.haveServiceUUID() &&
      advertisedDevice.isAdvertisingService(advertisingServiceUUID)
    ) {
      const auto address = advertisedDevice.getAddress().toString();
      if (clients.count(address) == 0) {
        const auto client = new VictorBleClient(new BLEAdvertisedDevice(advertisedDevice));
        client->onReport = [](const VictorBleReport report) { Serial.println(report.rawReport); };
        advertisedAddresses.push_back(address);
        clients[address] = client;
        Serial.println("Created client for server [" + String(address.c_str()) + "]");
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

void loop() {
  const auto now = millis();
  if (now - lastLoop > 5000) {
    lastLoop = now;
    if (advertisedAddresses.size() > 0) {
      for (auto address : advertisedAddresses) {
        const auto client = clients[address];
        client->connectRemoteServer();
        Serial.println("Connecting server [" + String(address.c_str()) + "]");
      }
      advertisedAddresses.clear();
    }

    std::vector<std::string> disconnectedAddresses = {};
    for (auto it = clients.begin(); it != clients.end(); ++it) {
      const auto client = clients[it->first];
      if (!client->isConnected()) {
        disconnectedAddresses.push_back(it->first);
      }
    }
    for (auto address : disconnectedAddresses) {
      clients.erase(address);
      Serial.println("Removed client [" + String(address.c_str()) + "]");
    }
    Serial.println("Clients [" + String(clients.size()) + "]");
  }
}
