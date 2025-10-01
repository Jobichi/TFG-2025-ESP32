#include "Sensors/Dht22Sensor.h"
#include "configCredentials.h"

Dht22Sensor::Dht22Sensor(
    uint8_t pin,
    unsigned long readPeriodMs,
    const char* friendlyName,
    const char* location)
: pin_(pin),
  readPeriodMs_(readPeriodMs),
  friendlyName_(friendlyName),
  dht_(pin, DHT22),
  location_(location) {}

bool Dht22Sensor::begin() {
    dht_.begin();
    healthy_ = true;
    lastReadMs_ = 0;
    return true;
}

void Dht22Sensor::loop() {
    const unsigned long now = millis();
    if (now - lastReadMs_ < readPeriodMs_) return;
    lastReadMs_ = now;

    const float h = dht_.readHumidity();
    const float t = dht_.readTemperature();

    if (isnan(h) || isnan(t)) {
        healthy_ = false;
        #if DEBUG
            Serial.println("Lectura sin valores en DHT22.");
        #endif
        return;
    }

    healthy_ = true;
    lastH_ = h;
    lastT_ = t;
}

std::map<String, float> Dht22Sensor::readValues() {
    std::map<String, float> data;
    data["temp"] = lastTemperatureC();
    data["hum"] = lastHumidity();
    return data;
}

String Dht22Sensor::stateString(){
    if(!healthy_) return "ERROR";
    return String(lastT_, 1) + "ºC " + String(lastH_, 0) + "%";
}