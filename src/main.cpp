#include <map>
#include <vector>
#include <Arduino.h>
#include <BLEDevice.h>
#include <VictorBleClient.h>

using namespace Victor;
using namespace Victor::Components;

/* Specify the Service UUID of Server */
static BLEUUID advertisingServiceUUID("0000abc0-0000-1000-8000-00805f9b34fb");
static BLEScan* scan = nullptr;

static std::vector<BLEAddress> advertisedAddresses = {};
static std::map<BLEAddress, VictorBleClient*> clients = {};

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (
      advertisedDevice.haveServiceUUID() &&
      advertisedDevice.isAdvertisingService(advertisingServiceUUID)
    ) {
      const auto address = advertisedDevice.getAddress();
      if (clients.count(address) == 0) {
        advertisedAddresses.push_back(address);
        clients[address] = new VictorBleClient(new BLEAdvertisedDevice(advertisedDevice));
      }
    }
  }
};

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
  if (advertisedAddresses.size() > 0) {
    for (auto it = advertisedAddresses.begin(); it != advertisedAddresses.end(); ++it) {
      const auto client = clients[*it];
      client->connectRemoteServer();
    }
    advertisedAddresses.clear();
  }

  for (auto client = clients.begin(); client != clients.end(); ++client) {
    String message = "Time since boot: " + String(millis() / 5000);
    if (client->second->heartbeat(message)) {
      Serial.println("Heartbeat message sent [" + message + "]");
    }
  }

  delay(5000);
}
