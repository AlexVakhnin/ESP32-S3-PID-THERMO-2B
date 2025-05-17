#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define I2C_CLOCK 9
#define I2C_DATA 10
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

extern double gOutputPwr; //результат вычислений PID
extern double gTargetTemp; //целевая температура
extern double currentTemp; //текущая температура по датчику
extern bool overShootMode; //далеко от цели..
extern int senserror;
extern bool overheat;
extern volatile bool tempfail;
extern int kind_error;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //тут передаем только ссылку I2C

String ds1_1 = ""; //для gTargetTem
String ds1_2 = ""; //для currentTemp
String ds1 = ""; //дисплей-строка 1
String ds2 = ""; //дисплей-строка 2

//char buffer[50]; //для sprintf()

// Обновление состояния дисплея
void disp_show_temp(){
  display.clearDisplay();

  display.setCursor(0,0); //координаты 1-й строки 1-го числа
  display.print(ds1_1);
  display.setCursor(64-8,0); //координаты 1-й строки 2-го числа
  display.print(ds1_2);

  display.setCursor(0,18); //координаты 2-й строки
  display.print(ds2);
  display.display();//show

}
// Обновление состояния дисплея
void disp_show(){
  display.clearDisplay();
  display.setCursor(0,0); //координаты 1-й строки 1-го числа
  display.print(ds1);
  display.setCursor(0,18); //координаты 2-й строки
  display.print(ds2);
  display.display();//show

}

//обновить дисплей T=200ms
void disp_refrash(){
  double dTemp = kind_error;
  if (senserror==0) dTemp=currentTemp;

  //ds1=String((int)gTargetTemp)+" "+String(dTemp);
  ds1_1 = String((int)gTargetTemp); ds1_2 = String(dTemp);


  if (overheat) ds2="OVERHEAT";
  else if (tempfail) ds2="TEMPFAIL";
  else ds2=String(overShootMode)+"  "+String(gOutputPwr/10)+"%";
  disp_show_temp(); //результат на дисплей
}

//SSD1306 OLED Init
void disp_setup(){
  //delay(1000);
  //ReDefine I2C Pins
  Wire.begin(I2C_DATA, I2C_CLOCK); //создается интерф I2C c новыми выводами
  // Setup display SSD1306
  //delay(1000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  //display.setRotation(3);
  display.display(); //show LOGO
  delay(1000);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.clearDisplay();
  //display.setCursor(0, 0);
  //display.setTextSize(8);  
  //display.print("1");

  //display.drawRect(0,43,32,83,WHITE);
  //display.drawRect(0,0,32,32,WHITE);
  display.display();//show
  //delay(1000);
  //disp_1();
  //disp_2();

}