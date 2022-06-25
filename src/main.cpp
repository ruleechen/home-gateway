#include <Arduino.h>
#include <HomeSpan.h>

const char* ssid = "104-2.4";
const char* password = "18950098099";

struct MySwitch : Service::Switch {
  int ledPin;
  SpanCharacteristic* power = nullptr ;

  MySwitch(int ledPin) : Service::Switch() {
    this->ledPin = ledPin;
    pinMode(ledPin, OUTPUT);
    power = new Characteristic::On();
  }

  bool update() {
    digitalWrite(ledPin, power->getNewVal());
    return true;
  }
};

void setup() {
  Serial.begin(115200);
  homeSpan.setStatusPin(BUILTIN_LED);
  homeSpan.setWifiCredentials(ssid, password);
  homeSpan.setPairingCode("18950098");
  homeSpan.begin(Category::Switches, "Victor-SWitch");

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Victor-Switch");
      new Characteristic::Manufacturer("RuleeSmart");
      new Characteristic::Model("Victor-Switch-ESP32");
      new Characteristic::SerialNumber("202206260023");
      new Characteristic::HardwareRevision("22.1.20");
      new Characteristic::FirmwareRevision("22.2.30");
    new MySwitch(BUILTIN_LED);
}

void loop() {
  homeSpan.poll();
}
