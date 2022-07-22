#include <map>
#include <vector>
#include <Arduino.h>
#include <BLEDevice.h>
#include <VictorBleClient.h>

using namespace Victor;
using namespace Victor::Components;

void ledOn() {
  digitalWrite(LED_BUILTIN, HIGH);
}
void ledOff() {
  digitalWrite(LED_BUILTIN, LOW);
}
void ledFlash(unsigned long ms = 100) {
  ledOn();
  delay(ms); // at least light for some time
  ledOff();
}

/* Specify the Service UUID of Server */
static BLEUUID advertisingServiceUUID("00001002-0000-1000-8000-00805f9b34fb");
static BLEScan* scan = nullptr;

static std::vector<std::string> advertisedAddresses = {};
static std::map<std::string, VictorBleClient*> clients = {};

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.printf("Scan found [%s]", advertisedDevice.toString().c_str()); Serial.println();
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
            Serial.printf("[%s] %s", serverAddress.toString().c_str(), item->raw); Serial.println();
            item = item->next;
          }
          if (notification->type == SERVER_NOTIFY_ON) {
            if (notification->args == "0") {
              ledOff();
            } else {
              ledOn();
            }
          }
        };
        advertisedAddresses.push_back(address);
        clients[address] = client;
        Serial.printf("Server match [%s]", address.c_str()); Serial.println();
      }
    }
  }
};

static unsigned long lastLoop = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  // ble init
  BLEDevice::init("Victor-Gateway");
  // init scan
  scan = BLEDevice::getScan();
  scan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  scan->setInterval(1349);
  scan->setWindow(449);
  scan->setActiveScan(true);
  scan->start(5, true);
  // done
  Serial.println();
  Serial.println("setup complete");
  ledFlash(500);
}

String message = "";
bool isEnterPressed = false;

void loop() {
  isEnterPressed = false;
  while (Serial.available()) {
    const auto ch = Serial.read();
    if (!isEnterPressed) {
      isEnterPressed = (ch == '\r');
      if (!isEnterPressed) {
        message += (char)ch;
      }
    }
  }

  if (isEnterPressed) {
    message.trim();
    for (auto it = clients.begin(); it != clients.end(); ++it) {
      const auto client = clients[it->first];
      client->send(message);
    }
    ledFlash();
    message = "";
  }

  const auto now = millis();
  if (now - lastLoop > 5000) {
    lastLoop = now;
    if (advertisedAddresses.size() > 0) {
      for (auto address : advertisedAddresses) {
        const auto client = clients[address];
        Serial.printf("[%s] connecting", address.c_str()); Serial.println();
        client->connectServer();
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
      Serial.printf("[%s] removed", address.c_str()); Serial.println();
    }
    if (clients.size() > 0) {
      ledFlash();
    }
  }
}
