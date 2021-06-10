#include "ESP8266WiFi.h"
void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

void loop(){
  Serial.println("Scanning WiFi Networks...");
  int count = WiFi.scanNetworks();
  if (count == 0){
    Serial.println("No WiFi Networks Found!");
  }else{
  // 输出单个WiFi热点信息
    for (int i = 0 ; i < count ; i++){
      //输出序号
      Serial.print(i + 1);
      Serial.print(":\t");
      //输出SSID
      Serial.print(WiFi.SSID(i));
      Serial.print("\t\t(");
      //输出RSSI
      Serial.print(WiFi.RSSI(i));
      Serial.print(")\t\t");
      //输出加密状态
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      //延时10毫秒
      delay(10);
    }
    Serial.println("*********************************************************");
    // 延时1秒
    delay(1000);
  }
  
}
