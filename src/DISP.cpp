#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define I2C_CLOCK 9
#define I2C_DATA 10
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

//extern double gOutputPwr; //результат вычислений PID
extern double gTargetTemp; //целевая температура
extern double currentTemp; //текущая температура по датчику
extern double gTargetTemp_b2;
extern double currentTemp_b2;
//extern bool overShootMode; //далеко от цели..
extern int senserror;
extern bool overheat;
extern volatile bool tempfail;
extern int senserror_b2;
extern bool overheat_b2;
extern volatile bool tempfail_b2;
extern int kind_error;
extern int kind_error_b2;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //тут передаем только ссылку I2C

String ds1 = ""; //дисплей-строка 1
String ds1_1 = ""; //для gTargetTem
String ds1_2 = ""; //для currentTemp

String ds2 = ""; //дисплей-строка 2
String ds2_1 = ""; //для gTargetTem_B2
String ds2_2 = ""; //для currentTemp_B2

//char buffer[50]; //для sprintf()

// Обновление состояния дисплея
void disp_show_temp(){
  display.clearDisplay();

  if(tempfail or overheat) {
    display.setCursor(0,0);
    display.print(ds1);
  } else {
    display.setCursor(0,0); //координаты 1-й строки 1-го числа
    display.print(ds1_1);
    display.setCursor(56,0); //координаты 1-й строки 2-го числа
    display.print(ds1_2);
  }

  if(tempfail_b2 or overheat_b2) {
    display.setCursor(0,18);
    display.print(ds2);
  } else {
    display.setCursor(0,18); //координаты 2-й строки 1-го числа
    display.print(ds2_1);
    display.setCursor(56,18); //координаты 2-й строки 2-го числа
    display.print(ds2_2);
  }

  display.display();//show
}

// Обновление состояния дисплея не в цикле LOOP()
void disp_show(){
  display.clearDisplay();
  display.setCursor(0,0); //координаты 1-й строки 1-го числа
  display.print(ds1);
  display.setCursor(0,18); //координаты 2-й строки
  display.print(ds2);
  display.display();//show
}

//обновить дисплей T=200ms, вызывается из LOOP()
void disp_refrash(){
  double dTemp = kind_error;
  double dTemp_b2 = kind_error_b2;
  if (senserror==0) dTemp=currentTemp;
  if (senserror_b2==0) dTemp_b2=currentTemp_b2;

  //ds1=String((int)gTargetTemp)+" "+String(dTemp);
  ds1_1 = String((int)gTargetTemp); ds1_2 = String(dTemp);
  ds2_1 = String((int)gTargetTemp_b2); ds2_2 = String(dTemp_b2);

  if (tempfail) ds1="TEMPFAIL"; //готовим переменные с ошибками..
  if (overheat) ds1="OVERHEAT"; //приоритет - OVERHEAT
  if (tempfail_b2) ds2="TEMPFAIL";
  if (overheat_b2) ds2="OVERHEAT";
  //else ds2=String(overShootMode)+"  "+String(gOutputPwr/10)+"%";
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