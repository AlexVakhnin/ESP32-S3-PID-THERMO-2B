#include <Arduino.h>
#include <WiFi.h>
#include "SPIFFS.h"

#define SW_PIN 8

//External Variables and functions

extern String ds1;
extern String ds2;
extern void disp_show();

extern const char* ssid1Path; //Web.cpp
extern const char* pass1Path;
extern const char* ssid2Path;
extern const char* pass2Path;
extern String readFile(fs::FS &fs, const char * path); //SPIFFS.cpp

//Список сетей (local variables)
String ssid1 = ""; //WIFI SSID
String pass1 = ""; //WIFI PASS
String ssid2 = ""; //WIFI SSID
String pass2 = ""; //WIFI PASS

boolean flag_ip = false;
boolean flag_apn = false;//флаг APN
IPAddress ip;

//Определяем условный номер сети(ssid) из списка сетей.
//результат 0-нет сетей из списка ; 1=сеть ssid1 ; 2- сеть ssid2
int wifi_scan(){
    int res = 0; 
    Serial.println("Scan start");
    ds1 = "Scan start";disp_show();

    // WiFi.scanNetworks will return the number of networks found.
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");
    ds2 = "Scan done";disp_show();
    if (n==0){
      Serial.println("no networks found");
      ds1 = "no found";disp_show();
    }
    else {
        for (int i = 0; i < n; ++i) {
            String s=WiFi.SSID(i);
            Serial.println(s);
            if (s.equals(ssid1)){res=1;break;}
            if (s.equals(ssid2)){res=2;break;}
            delay(10);
        }
        Serial.println("**********************");
    }
    WiFi.scanDelete();
    return res;
}

void wifi_init(){

  //загрузим список сетей из файловой системы
  ssid1 = readFile(SPIFFS, ssid1Path);
  pass1 = readFile(SPIFFS, pass1Path);
  ssid2 = readFile(SPIFFS, ssid2Path);
  pass2 = readFile(SPIFFS, pass2Path);

  String ssid="No Netw..";
  String pass="";
  int ind = wifi_scan(); //номер сети из списка
  if (ind==1){ssid=ssid1;pass=pass1;}
  else if (ind==2){ssid=ssid2;pass=pass2;}

  //ssid = "_Tenda";  //debug !!!!!!!!!
  //pass = "tenda123";

  Serial.println("ssid="+ssid);
  ds1 = ssid;disp_show();//SSID на дисплей
  WiFi.begin(ssid, pass);
  int count = 12;
  while (WiFi.status() != WL_CONNECTED) {
      count--;
      delay(1000);
      Serial.print(".");//индикация
      if (count <= 0){
        flag_apn = true;
        WiFi.disconnect(true);
        delay(2000);
        break;  //выходим из WHILE
      }
  }
  if(flag_apn) {
    //создаем AP
    Serial.println("Creating Access Point..");
    WiFi.softAP("Thermo", "thermo123"); // NULL sets an open Access Point
    ip = WiFi.softAPIP();
    Serial.print("AP IP address: ");Serial.println(ip);
    ds2 = "thermo123";disp_show();
  } else {
    Serial.println();
    ip = WiFi.localIP();
  }
  Serial.print("IP address: ");Serial.println(ip);
  String s_ip = "IP."+String(ip[2])+"."+String(ip[3]); //на дисплей
  ds1 = s_ip;disp_show();
  delay(4000);

} 

//если APN , выключаем его по кнопке
void apn_stop(){

  if(flag_apn){ //заглушка если APN
    ds2 = "APN IS ON";disp_show();
    pinMode(SW_PIN, INPUT_PULLUP); //инициализируем порт
    //delay(300);
    while(1){
      if (digitalRead(SW_PIN) == LOW) {
        //flag_apn = false;
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        delay(300);
        break;
      }      
      delay(100);
    }
  }
}

//повторный коннект, в случае потери связи, вызывается из loop()
void handle_wifi(){
  if (WiFi.status() != WL_CONNECTED)  {
    Serial.print(millis());
    Serial.println(" Reconnecting to WiFi...");
    //ds1="Reconnect";//на дислей
    //ds2=String(millis());
    WiFi.disconnect();
    WiFi.reconnect();
    flag_ip = true;
  } else {
    if (flag_ip){
      IPAddress ip = WiFi.localIP();
      Serial.print("IP address: ");Serial.println(ip);
      //web_reset();
      //web_init();
      //ds1 = "IP.."+String(ip[2])+"."+String(ip[3]);// на дисплей
      flag_ip = false;
    }
  }

}