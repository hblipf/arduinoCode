#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SSD1306.h>

SSD1306 display(0x3c,2, 14);//实例化oled
//wifi定义
const char* ssid = "LGCX_LEAPFLY";
const char* password = "lgcx12345678";

void setup() {
  //初始化串口
  Serial.begin(115200);
  Serial.println("----");
  //初始化OLED
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  //初始化网络
  initWIFI();
  Serial.print("IP Address:"+WiFi.localIP());
}
void loop() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0,0,"ABC哈");
  display.display();
  delay(2000);
}
//连接WIFI
void initWIFI() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //等待连接
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}
