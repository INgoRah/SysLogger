#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <SysLogger.h>
#include "config.h"

SysLogger Log;
const char* host = "esp";

/* wait if 0, not waiting for connection (trying once). Else wait specifies
   timout in 100 ms steps. */
void wifi_connect(char en, int wait) {
  int i = 0;

  if (en == 1) {
    if (WiFi.status() == WL_CONNECTED)
      return;
    WiFi.begin(ssid, password);
    if (wait != 0) {
      Serial.print ("connecting");
      Serial.flush();
    }
    /* trying to connect */
    while (WiFi.status() != WL_CONNECTED) {
        if (wait-- <= 0)
          return;      
        delay(100);
        if (i++ % 10 == 0)
          Serial.print (".");Serial.flush();
    }
    Serial.printf ("IP %s\n", WiFi.localIP().toString().c_str());
  } else
    WiFi.disconnect(1);
}

void setup(void){
  Serial.begin(115200);
  Serial.println ("starting...");
  WiFi.mode(WIFI_STA);
 
  // connecting....timout 5 mins
  wifi_connect(1, 5*60*10);
  MDNS.begin(host);
  MDNS.addService("http", "tcp", 80);

  // define target IP of syslog daemon
  IPAddress ip(192,168,178,25);
  Log.init(ip);
  Log.println ("syslog init done");
}

/* send out alive every 10 secs */
#define STATUS_POLL_CYCLE (1000 * 10)

void loop(void){
  static unsigned long status_poll = 0;

  if (millis() - status_poll >= STATUS_POLL_CYCLE) {
      status_poll = millis();
      Log.printf("alive\n");
  }
}
