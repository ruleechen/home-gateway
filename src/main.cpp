#include <Arduino.h>
#include <BLEDevice.h>

/* Specify the Service UUID of Server */
static BLEUUID advertisingServiceUUID_short("0000abc0-0000-1000-8000-00805f9b34fb");
static BLEUUID advertisingServiceUUID_long( "0000fff0-0000-1000-8000-00805f9b34fb");

static boolean doConnect = false;
static boolean isConnected = false;
static boolean doScan = false;

static BLEScan* scan = nullptr;
static BLEAdvertisedDevice* advertisedDevice = nullptr;
static BLERemoteCharacteristic* remoteCharacteristicWritable = nullptr;

static void notifyCallback(BLERemoteCharacteristic* remoteCharacteristic, uint8_t* data, size_t length, bool isNotify) {
  const auto strData = String(data, length);
  Serial.print("[NotifyCallback] from characteristic ");
  Serial.print(remoteCharacteristic->getUUID().toString().c_str());
  Serial.print("data: ");
  Serial.println(strData);
  if (strData == "hello" && remoteCharacteristicWritable != nullptr) {
    String newValue = "RuleeSmart";
    remoteCharacteristicWritable->writeValue(newValue.c_str(), newValue.length());
  }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* client) {
    Serial.println("[BLEClient > onDisconnect]");
  }
  void onDisconnect(BLEClient* client) {
    isConnected = false;
    remoteCharacteristicWritable = nullptr;
    Serial.println("[BLEClient > onDisconnect]");
  }
};

void connectCharacteristic(BLERemoteCharacteristic* remoteCharacteristic) {
  /* Read the value of the characteristic */
  /* Initial value is 'Hello, World!' */
  Serial.print("Connecting Characteristic ------> ");
  Serial.println(remoteCharacteristic->getUUID().toString().c_str());

  if (remoteCharacteristic->canWrite() && remoteCharacteristicWritable == nullptr) {
    remoteCharacteristicWritable = remoteCharacteristic;
    Serial.println(" > can write");
  }

  if (remoteCharacteristic->canRead()) {
    std::string value = remoteCharacteristic->readValue();
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

void connectService(BLERemoteService* pRemoteService) {
  Serial.println();
  Serial.print("Connecting Service ------> ");
  Serial.println(pRemoteService->getUUID().toString().c_str());
  std::map<std::string, BLERemoteCharacteristic*>* remoteCharacteristics = pRemoteService->getCharacteristics();
  for (auto it = remoteCharacteristics->begin(); it != remoteCharacteristics->end(); it++) {
    connectCharacteristic(it->second);
  }
}

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(advertisedDevice->getAddress().toString().c_str());

  BLEClient* client = BLEDevice::createClient();
  client->setClientCallbacks(new MyClientCallback());
  Serial.println(" - Created client");

  /* Connect to the remote BLE Server */
  client->connect(advertisedDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

  /* Obtain reference to the service we are after in the remote BLE server */
  std::map<std::string, BLERemoteService*>* remoteServices = client->getServices();
  for (auto it = remoteServices->begin(); it != remoteServices->end(); it++) {
    connectService(it->second);
  }
  isConnected = true;
  return true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  /* Called for each advertising BLE server. */
  void onResult(BLEAdvertisedDevice advDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advDevice.toString().c_str());
    /* We have found a device, let us now see if it contains the service we are looking for. */
    if (
      advDevice.haveServiceUUID() &&
      (advDevice.isAdvertisingService(advertisingServiceUUID_short) ||
      advDevice.isAdvertisingService(advertisingServiceUUID_long))
    ) {
      scan->stop();
      advertisedDevice = new BLEAdvertisedDevice(advDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("Victor-Gateway");
  scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  scan->setInterval(1349);
  scan->setWindow(449);
  scan->setActiveScan(true);
  scan->start(5, false);
}

void loop() {
  if (doConnect == true) {
    doConnect = false;
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
  }

  if (isConnected) {
    String newValue = "Time since boot: " + String(millis() / 5000);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    remoteCharacteristicWritable->writeValue(newValue.c_str(), newValue.length());
  } else if (doScan) {
    scan->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }

  delay(5000); /* Delay between loops */
}
