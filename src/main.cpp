#include <map>
#include <vector>
#include <Arduino.h>
#include <BLEAddress.h>
#include <BLEDevice.h>

/* Specify the Service UUID of Server */
static BLEUUID advertisingServiceUUID("0000abc0-0000-1000-8000-00805f9b34fb");

static BLEScan* scan = nullptr;
static BLEClient* client = nullptr;

static std::vector<BLEAddress> advertisedAddresses = {};
static std::map<BLEAddress, BLEAdvertisedDevice*> advertisedDevices = {};
static std::map<BLEAddress, BLERemoteCharacteristic*> remoteCharacteristicWritable = {};

static void notifyCallback(BLERemoteCharacteristic* remoteCharacteristic, uint8_t* data, size_t length, bool isNotify) {
  const auto strData = String(data, length);
  Serial.print("[NotifyCallback] from characteristic ");
  Serial.print(remoteCharacteristic->getUUID().toString().c_str());
  Serial.print("data: ");
  Serial.println(strData);
  if (strData == "hello") {
    String newValue = "RuleeSmart";
    remoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* c) {
    Serial.println("[BLEClient > onDisconnect]");
  }
  void onDisconnect(BLEClient* c) {
    Serial.println("[BLEClient > onDisconnect]");
  }
};

void connectRemoteCharacteristic(BLEAddress address, BLERemoteCharacteristic* remoteCharacteristic) {
  /* Read the value of the characteristic */
  /* Initial value is 'Hello, World!' */
  Serial.print("Connecting Characteristic ------> ");
  Serial.println(remoteCharacteristic->getUUID().toString().c_str());

  if (remoteCharacteristic->canWrite()) {
    remoteCharacteristicWritable[address] = remoteCharacteristic;
    Serial.println(" > can write");
  }

  if (remoteCharacteristic->canRead()) {
    const auto value = remoteCharacteristic->readValue();
    Serial.println(" > can read");
    Serial.print(" > value is: [");
    Serial.print(value.c_str());
    Serial.println("]");
    // only for read from server
    if (remoteCharacteristic->canNotify()) {
      remoteCharacteristic->registerForNotify(notifyCallback);
      Serial.println(" > registerForNotify");
    }
  }
}

void connectRemoteService(BLEAddress address, BLERemoteService* remoteService) {
  Serial.println();
  Serial.print("Connecting Service ------> ");
  Serial.println(remoteService->getUUID().toString().c_str());
  const auto remoteCharacteristics = remoteService->getCharacteristics();
  for (auto it = remoteCharacteristics->begin(); it != remoteCharacteristics->end(); it++) {
    connectRemoteCharacteristic(address, it->second);
  }
}

void connectRemoteServer(BLEAdvertisedDevice* advertisedDevice) {
  auto address = advertisedDevice->getAddress();
  Serial.print("Forming a connection to ");
  Serial.println(address.toString().c_str());

  /* Connect to the remote BLE Server */
  client->connect(advertisedDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

  /* Obtain reference to the service we are after in the remote BLE server */
  const auto remoteServices = client->getServices();
  for (auto it = remoteServices->begin(); it != remoteServices->end(); it++) {
    connectRemoteService(address, it->second);
  }
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  /* Called for each advertising BLE server. */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    /* We have found a device, let us now see if it contains the service we are looking for. */
    if (
      advertisedDevice.haveServiceUUID() &&
      advertisedDevice.isAdvertisingService(advertisingServiceUUID)
    ) {
      const auto address = advertisedDevice.getAddress();
      if (advertisedDevices.count(address) == 0) {
        advertisedAddresses.push_back(address);
        advertisedDevices[address] = new BLEAdvertisedDevice(advertisedDevice);
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("Victor-Gateway");
  // init client
  client = BLEDevice::createClient();
  client->setClientCallbacks(new MyClientCallback());
  // init scan
  scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  scan->setInterval(1349);
  scan->setWindow(449);
  scan->setActiveScan(true);
  scan->start(5, true);
}

void loop() {
  if (advertisedAddresses.size() > 0) {
    for (auto it = advertisedAddresses.begin(); it != advertisedAddresses.end(); ++it) {
      const auto advertisedDevice = advertisedDevices[*it];
      connectRemoteServer(advertisedDevice);
    }
    advertisedAddresses.clear();
  }

  if (client->isConnected()) {
    String newValue = "Time since boot: " + String(millis() / 5000);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    for (auto it = remoteCharacteristicWritable.begin(); it != remoteCharacteristicWritable.end(); ++it) {
      it->second->writeValue(newValue.c_str(), newValue.length());
    }
  }

  delay(5000);
}
