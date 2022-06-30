#include <Arduino.h>

// #include <HomeSpan.h>

// const char* ssid = "104-2.4";
// const char* password = "18950098099";

// struct MySwitch : Service::Switch {
//   int ledPin;
//   SpanCharacteristic* power = nullptr ;

//   MySwitch(int ledPin) : Service::Switch() {
//     this->ledPin = ledPin;
//     pinMode(ledPin, OUTPUT);
//     power = new Characteristic::On();
//   }

//   bool update() {
//     digitalWrite(ledPin, power->getNewVal());
//     return true;
//   }
// };

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "8c2ddf54-0847-4ac7-9490-b7abcabb09c3"
#define CHARACTERISTIC_UUID "d7d829f8-c06a-4247-a8b0-074e9a4e3b78"

BLECharacteristic* pCharacteristic;

bool deviceConnected = false;
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("[BLE > onConnect]");
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("[BLE > onDisconnect]");
  }
};

class MyChaCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++) {
        Serial.print(value[i]);
      }
      Serial.println();
      Serial.println("*********");
    }
  }
};

void initBleServer() {
  BLEDevice::init("Victor-Gateway");
  auto pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  auto pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic->setCallbacks(new MyChaCallbacks());
  pCharacteristic->setValue("Hello!");
  pService->start();

  auto pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->start();

  Serial.println("Waiting a client connection to notify...");
}

void initHomeSpan() {
  // homeSpan.setStatusPin(BUILTIN_LED);
  // homeSpan.setWifiCredentials(ssid, password);
  // homeSpan.setPairingCode("18950098");
  // homeSpan.begin(Category::Switches, "Victor-SWitch");

  // new SpanAccessory();
  //   new Service::AccessoryInformation();
  //     new Characteristic::Identify();
  //     new Characteristic::Name("Victor-Switch");
  //     new Characteristic::Manufacturer("RuleeSmart");
  //     new Characteristic::Model("Victor-Switch-ESP32");
  //     new Characteristic::SerialNumber("202206260023");
  //     new Characteristic::HardwareRevision("22.1.20");
  //     new Characteristic::FirmwareRevision("22.2.30");
  //   new MySwitch(BUILTIN_LED);
}

void setup() {
  Serial.begin(115200);
  initBleServer();
  initHomeSpan();
}

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void loop() {
  // homeSpan.poll();
  if (deviceConnected) {
    auto now = millis();
    if (now - lastTime > timerDelay) {
      lastTime = now;
      BLEDevice::startAdvertising();
      // update characteristic value and notify connected client
      pCharacteristic->setValue(std::to_string(lastTime));
      pCharacteristic->notify();
      Serial.println("[BLE > notify]" + String(lastTime));
    }
  }
}
