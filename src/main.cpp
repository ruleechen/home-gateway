#include <Arduino.h>
#include <HomeSpan.h>

const char *ssid = "PHICOMM_B618";
const char *password = "18950098099";

void setup() {
  Serial.begin(115200);
  homeSpan.setStatusPin(BUILTIN_LED);
  homeSpan.setWifiCredentials(ssid, password);
  homeSpan.setPairingCode("18950098");
  homeSpan.begin(Category::Switches, "Victor-SWitch");
  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  new Service::Switch();
  new Characteristic::On(true);
}

void loop() {
  homeSpan.poll();
}
