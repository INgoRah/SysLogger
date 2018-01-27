#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include "Print.h"

class SysLogger : public Print {
  private:
    WiFiUDP _udp;
    IPAddress _ip;
    int _idx;
    int _last;
    int _bufMax;
    time_t _now;
    struct tm  *_tm;
    String* _log;
    size_t write(uint8_t c) {
      return Serial.write (&c, 1);
    }

  public:
    SysLogger() : _bufMax(0), _idx(0), _tm(NULL), _last(-1), _log(NULL) {};

    void init(IPAddress ip);
    void init(IPAddress ip, int logBufSize);
    void init(int logBufSize);
    size_t write(const uint8_t *buffer, size_t size) override;
    int read(String* log);
    const String last();
};
