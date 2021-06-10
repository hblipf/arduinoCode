//爆改车间主任 
//ESP12+TFT显示屏（1.3寸 SPI接口 ST7789 240x240）DIY太空人表盘代码
//开发板NodeMCU
//SCL-->D5
//SDA-->D7
//RSE-->D4
//DC -->D6
//TFT_eSPI 显示驱动库
//需先配置wifi连接信息
//NTP网络获取时间
//知心天气免费版接口获取温度，需先官网注册账号获取KEY。填入代码。
//新手勤快搜索，老手多指教。
//“爆改车间”微信公众号获取主任更多代码

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>     //定时器库
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>

#include "bmp.h"
TFT_eSPI tft = TFT_eSPI();
uint16_t back = tft.color565(248,248,255);

Ticker flipper;        //创建定时器
Ticker flipper_temp;        //创建定时器
Ticker flipper_heart;
Ticker flipper_cube;

#ifndef STASSID
#define STASSID "LGCX_LEAPFLY"      //输入Wi-Fi名
#define STAPSK  "lgcx12345678"      //输入Wi-Fi密码
#endif
const char* ssid     = STASSID;
const char* password = STAPSK;

#include <TZ.h>
#include <ESPPerfectTime.h>      //NTP网络时间服务器获取库
const char *ntpServer = "ntp.sjtu.edu.cn";     //上海交通大学NTP服务器


#define MAX_CONTENT_SIZE 2000
WiFiClient client;
HTTPClient http;
char response[MAX_CONTENT_SIZE];

const char* HOST = "api.seniverse.com";
const char* APIKEY = "SajAPghlam7gUaSIe";    //输入知心天气免费接口密钥，官网注册获取
const char* CITY = "beijing";     //输入城市


typedef struct UserData{
    char city[16];
    char weather[32];
    char temp[16];
    char code[16];
}data;

data d;


#define YEAR 2000 
#define MOTH 1
#define DAY  1
#define xingqi 6 
char const days_of_moth[] = {31,28,31,30,31,30,31,31,30,31,30,31};

void printTime(struct tm *tm, suseconds_t usec) {
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d.%06ld\n",
                tm->tm_year + 1900,
                tm->tm_mon + 1,
                tm->tm_mday,
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec,
                usec);
}

//-------------

void clearResponseBuffer(){
    memset(response, 0, MAX_CONTENT_SIZE);
    memset(d.city,0,sizeof(d.city));
    memset(d.weather,0,sizeof(d.weather));
    memset(d.temp,0,sizeof(d.temp));
    memset(d.code,0,sizeof(d.code));
}

data getdata(){
    
    char error[5] = "NULL";
    clearResponseBuffer();
    
    if(sendRequest(HOST,CITY,APIKEY)){
        Serial.println("Request success!\n\n");
    }
    if(parseData(d)){
        return d;
    }else{
        strcpy(d.city,error);
        strcpy(d.weather,error);
        strcpy(d.temp,error);
        strcpy(d.code,error);
        
        return d;
    }
}

bool parseData(data &d){
    
    DynamicJsonDocument doc(1024);
  
    DeserializationError error = deserializeJson(doc, response);
    if (error){
      Serial.println("JSON parse failed!\n");
      return false;
    }
      
    strcpy(d.city,doc["results"][0]["location"]["name"]);
    strcpy(d.weather,doc["results"][0]["now"]["text"]);
    strcpy(d.temp,doc["results"][0]["now"]["temperature"]);
    strcpy(d.code,doc["results"][0]["now"]["code"]);
    
    return true;
}

bool sendRequest(const char* host, const char* cityid, const char* apiKey) {
    
    //const char *response;
    char *Url = (char *)malloc(0x100);
    String payload;
    
    sprintf(Url,"%s%s%s%s%s%s%s","http://",host,"/v3/weather/now.json?key=",apiKey,"&location=",cityid,"&language=en");
    //Serial.printf("GET URL: %s\n",Url);
    
    
    http.begin(Url);
    int httpCode = http.GET();
    if(httpCode>0){
        Serial.printf("[HTTP] GET... code: %d\n",httpCode);
    
        if(httpCode == 200){
            payload = http.getString();
            //Serial.println(payload);
        }else{
            delay(5000);
            sendRequest(HOST,CITY,APIKEY);
        }
    }else{
        delay(5000);
        sendRequest(HOST,CITY,APIKEY);
    }
    strcpy(response,payload.c_str());       // convert to const char *
    free(Url);
    return true;
}

void gettemp(){
  Serial.println(getdata().temp);
  Serial.println(getdata().code);
  tft.setCursor(100,5 );
  tft.setTextSize(2);
  tft.setTextFont(1);
  tft.print(getdata().temp);

}

//-----星期判函数 

int IsLeapYear(short int y)//判断闰年函数，返回1是闰年，返回0不是闰年
{
    return (((y%4 == 0)&&(y%100 != 0))||(y%400 == 0));
}

char GetWeek(short int year,char moth, char day)//计算带入的日期是星期几，返回值就是星期几
{
    short int days = 0;//用来计算两个日期相差天数
    short int i = 0;
        
    days = days + 365*(year - YEAR);//求相差几个整年天数
    if(year > YEAR)//计算到当前年份之间有没有闰年，有闰年天数多加一天
    {
        for( i = YEAR;i < year;i++)
        {
            if(IsLeapYear(i))
                days++;
        }   
    }   
        
    for(i = 1;i < moth;i++)//求需要加几个月份
    {   
        days = days + days_of_moth[i-1];
    }
    if((moth > 2)&&(IsLeapYear(year)))//要求的当年为闰年，且月份超过2月，多加一天
        days++;

    days  = days + (day - DAY);//求得两个日期之间相差天数

    days = days%7;
    days = days + xingqi;
    if(days > 7)
        days = days - 7;
    return days;
}



uint16_t hh,mm,ss,yy,mo,md,weekn;
void gettime(){        //定时器定时器获取网络时间
  struct tm *tm = pftime::localtime(nullptr);

  int w_mon ;
  if(tm->tm_mon == 1)w_mon=13;
  else if (tm->tm_mon == 2)w_mon=14;
  else w_mon=tm->tm_mon;
  weekn = GetWeek(tm->tm_year + 1900,tm->tm_mon + 1,tm->tm_mday);
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d %02d\n",
              tm->tm_year + 1900,
              tm->tm_mon + 1,
              tm->tm_mday,
              tm->tm_hour,
              tm->tm_min,
              tm->tm_sec,
              weekn
              );
  hh = tm->tm_hour;
  mm = tm->tm_min;
  ss = tm->tm_sec;
  yy = tm->tm_year + 1900;
  mo = tm->tm_mon + 1;
  md = tm->tm_mday;

  
  tft.setCursor(177,68);
  tft.setTextSize(4);
  tft.setTextFont(1);
  tft.setTextColor(TFT_BLACK,back);
  if(ss<10)tft.print("0");
  tft.print(ss);
  
  tft.setCursor(25, 50);
  tft.setTextSize(1);
  tft.setTextFont(7);
  if(hh<10)tft.print("0");
  tft.print(hh);
  tft.print(":");
  if(mm<10)tft.print("0");
  tft.print(mm);

  tft.setCursor(20, 195);
  tft.setTextSize(2);
  tft.setTextFont(1);
  tft.print(yy);
  tft.print("-");
  if(mo<10)tft.print("0");
  tft.print(mo);
  tft.print("-");
  if(md<10)tft.print("0");
  tft.print(md);

  tft.pushImage(150, 120, 22, 22,week[7]);
  tft.pushImage(172, 120, 22, 22,week[8]);
  tft.pushImage(194, 120, 22, 22,week[weekn-1]);
    
}
uint8_t i=0;
void taikongren(){
  tft.pushImage(70, 100, 70, 70,bmp[i]);
  i++;
  if(i>23)i=0;
}
uint8_t heart_n;
void heart_t(){
  tft.pushImage(180, 190, 42, 33,heart[heart_n]);
  heart_n++;
  if(heart_n>1)heart_n=0;
}

uint8_t cube_n=0;
void cube(){
  tft.drawFastVLine(10+cube_n,13,10,TFT_BLACK);
  cube_n++;
  if(cube_n>60){
    cube_n=0;
    tft.fillRoundRect(10,13,61,10,0,back);
  }
}
void setup(void) {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  pftime::configTzTime(TZ_Asia_Shanghai, ntpServer);

  tft.init();
  tft.fillScreen(back);
  tft.setSwapBytes(true);
  tft.fillRect(0, 40, 240,2, TFT_BLACK);
  tft.fillRect(80, 0, 2,40, TFT_BLACK);
  tft.fillRect(0, 180, 240,2, TFT_BLACK);
  tft.fillRect(160, 180, 2,40, TFT_BLACK);
  tft.fillRect(80, 220, 80,2, TFT_BLACK);
  tft.fillRect(80, 220, 2,20, TFT_BLACK);


  flipper.attach(1, gettime);
  flipper_temp.attach(0.06,taikongren);
  flipper_heart.attach(0.5,heart_t);
  flipper_cube.attach(0.1,cube);

}

void loop() {
  if(ss%10==0){
    Serial.println(getdata().temp);
    Serial.println(d.code);
    tft.setCursor(105,5);
    tft.setTextSize(3);
    tft.setTextFont(1);
    tft.print(d.temp);
    tft.pushImage(105+tft.textWidth(getdata().temp, 1)+3, 3, 24, 30,tempicon[0]);
    tft.pushImage(180, 3, 38, 30,codeicon[0]); //天气图标是假的，图标太多了，可自行从知心天气下载图标取模后添加。
    while(ss%10==0)yield();
  }
}
