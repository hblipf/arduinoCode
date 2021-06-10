#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//--------------------
//此处为修改OLED屏幕类型的处
//0.96寸的是128X64,0.91寸的是128X32，根据自己使用的尺寸修改需要注释掉的命令

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // ESP32 Thing, HW I2C with pin remapping
//---------------------

void setup(void) {
  u8g2.begin();
  u8g2.enableUTF8Print();
}

void loop(void) {
  u8g2.clearBuffer();// clear the internal memory
  u8g2.setFont(u8g2_font_wqy13_t_gb2312); // choose a suitable font
  //u8g2.setCursor(30, 13);
  u8g2.setCursor(30, 25);
  u8g2.print("暴改车间主任");
  //u8g2.setCursor(15, 30);
  u8g2.setCursor(15, 45);
  u8g2.print("baogaiMCU开发板");
  
  u8g2.sendBuffer();// transfer internal memory to the display
  delay(1000);  
}
