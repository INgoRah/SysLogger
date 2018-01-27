/*
  WiFi Syslog server

 This sketch receives syslog messages (UDP port 514) and prints them on the console as well as
 stores them into a string for read out via webserver.

 created 27 Januar 2018 by INgo.Rah@gmx.net
 */
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <time.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

char ssid[] = "***"; //  your network SSID (name)
char pass[] = "***";    // your network password (use for WPA, or use as key for WEP)

char packetBuffer[255]; //buffer to hold incoming packet

WiFiUDP Udp;
String logData;
time_t _now;
struct tm  *_tm;
static char bootTime[16];

// (utc+) TZ in hours
#define TZ 1
// use 60mn for summer time in some countries
#define DST_MN  0
#define TZ_SEC ((TZ)*3600)
#define DST_SEC ((DST_MN)*60)
void timeSetup()
{
  unsigned timeout = 10000;
  unsigned start = millis();
  struct tm *currentTime;

  while (millis() - start < timeout) {
    configTime(TZ_SEC, DST_SEC, "pool.ntp.org");
    _now = time(nullptr);
    if (_now > (2016 - 1970) * 365 * 24 * 3600) {
      delay(50);
      _now = time(nullptr);
      currentTime = localtime(&_now);
      sprintf(bootTime, "%02d.%02d %02d:%02d",
        currentTime->tm_mday, currentTime->tm_mon + 1,
        currentTime->tm_hour, currentTime->tm_min);
        logData = "== Up since ";
        logData += bootTime;
        logData += " ==";
      return;
    }
    delay(100);
  }
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  
  WiFi.begin(ssid, pass);
  Serial.print ("connecting");
  int wait = 100, i;
  while (WiFi.status() != WL_CONNECTED) {
      if (wait-- <= 0) {
        if (i > 0) {
          Serial.print("failed!");
          WiFi.begin(ssid, pass);
          wait = 100;
        }
      }
      delay(100);
      if (i++ % 10 == 0)
        Serial.print (".");Serial.flush();
  }
  Serial.printf ("IP %s\n", WiFi.localIP().toString().c_str());
  logData = "";
  server.on("/", []() {
    server.send(200, "text/plain", logData);
  });
  server.on("/clear", []() {
    logData = "== Up since ";
    logData += bootTime;
    logData += " ==\n";
    server.send(200, "text/plain", logData);
  });
  httpUpdater.setup(&server);
  server.begin(8080);
  timeSetup();
  // if you get a connection, report back via serial:
  Udp.begin(514);
}

void loop() {
  server.handleClient();

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      char s_tm[9];

      packetBuffer[len] = 0;
      _now = time(nullptr);
      _tm = localtime(&_now);
      sprintf(s_tm, "%02d:%02d:%02d ", _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
      logData += s_tm;
      logData += packetBuffer;
      logData += "\n";
      Serial.println(packetBuffer);
    }
  }
}

