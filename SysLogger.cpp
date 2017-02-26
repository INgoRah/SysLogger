#include <SysLogger.h>

size_t SysLogger::write(const uint8_t *buffer, size_t size) {
  Serial.write(buffer, size);
  if (_ip != 0) {
    if (!_buffered) {
      _udp.beginPacket(_ip, 514);
      _udp.write("<0> ");
      _buffered = true;
    }
    int ret = _udp.write(buffer, size);
    /** \todo only send if CR detected */ 
    if (true) {
      _udp.endPacket();
      _buffered = false;
    }
  }
  return size;
}

void SysLogger::init(IPAddress ip) {
    _ip = ip;
    _udp.begin(514);
}
