#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Wire.h>
#include <SSD1306.h>
#include <SimpleDHT.h>

SSD1306 display(0x3c,2, 14);//实例化oled
int pinDHT11 = 5;//DHT11引脚定义 IO5
SimpleDHT11 dht11(pinDHT11);//实例化dht11
//wifi定义
const char* ssid = "LGCX_LEAPFLY";
const char* password = "lgcx12345678";

ESP8266WebServer server(80);
const int led = 2 ;

byte temperature = 0;//定义温度并初始化0
byte humidity = 0;//定义湿度并初始化0

void setup() {
  //初始化串口
  Serial.begin(115200);
  Serial.println("");
  //初始化OLED
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  //初始化网络
  initWIFI();
  
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
  //初始化WebServer
  server.on("/", homepage);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  Serial.print((int)temperature); Serial.print(" *C, ");//串口打印出温度
  Serial.print((int)humidity); Serial.println(" H");//串口打印出湿度
  
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(25, 10, "T:"+String(temperature)+" *C ");
  display.drawString(25, 30, "H:"+String(humidity)+" H");
  display.display();
  delay(2500);
  server.handleClient();
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

void homepage() {
  server.send(200, "text/plain","this is ESP8266!data:[{T:"+String(temperature)+"},{H:"+String(humidity)+"}]");
  Serial.println("用户访问了主页,this is ESP8266!");
}
