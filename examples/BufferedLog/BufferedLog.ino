/*
  WiFi Syslog client. Logging messages to a syslog server and into a buffer.
  It logs every second a message and dumps the buffer every 10 seconds 
  to the console.
 
  created 27 Januar 2018 by INgo.Rah@gmx.net
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <SysLogger.h>

const char* ssid = "***";
const char* password = "***";

SysLogger Log;

#define MAX_LOG 10

/* wait if 0, not waiting for connection (trying once). Else wait specifies
   timeout in 100 ms steps. */
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
  // define target IP of syslog daemon
  Log.init(IPAddress(192,168,178,22), MAX_LOG);
  Log.println ("syslog init done");
}

/* send out alive every 1 secs*/
#define STATUS_POLL_CYCLE (1000 * 1)

void loop(void){
  static unsigned long statusPoll = 0;
  static unsigned long logDump = 0;

  if (millis() - statusPoll >= STATUS_POLL_CYCLE) {
      statusPoll = millis();
      Log.printf("alive\n");
  }
  if (millis() - logDump >= 10 * STATUS_POLL_CYCLE) {
      logDump = millis();
      String s[MAX_LOG];
      int cnt, i;
      String content;

      cnt = Log.read(s);
      for (i = 0; i < cnt; i++)
        Serial.println(s[i]);
  }
}
