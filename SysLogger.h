#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

class SysLogger : public Print {
  private:
    WiFiUDP _udp;
    IPAddress _ip;
    bool _buffered;
    size_t write(uint8_t c) {
      return Serial.write (&c, 1);
    }

  public:
    void init(IPAddress ip);
    size_t write(const uint8_t *buffer, size_t size) override;
};
