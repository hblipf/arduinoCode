#include <ESP8266WiFi.h>
#define ssid "ESP8266"
#define pass "123456789"
void setup() {
  //波特率为115200
  Serial.begin(115200);
  //wifi模式
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid,pass);
  IPAddress ip=WiFi.softAPIP();
  Serial.println(ip);
}

void loop() {
  Serial.printf("num:",WiFi.softAPgetStationNum());
  delay(5000);
}
