#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

#define SENS_PERIOD 150

extern double currentTemp; //температура термопары
extern double currentTemp_b2;
//extern bool overShootMode;
extern unsigned long time_now;

#define MAXDO   16  //MISO - data from slave  <--  (pull-up resistor 1-4.7 kom !!!)
#define MAXCLK  14  //SC - serial clock to slave -->
#define MAXCS   15  //SS - serial select to slave -->
#define MAXCS_B2   15  // !!!

// initialize the Thermocouple
//Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO); //это программный SPI

SPIClass myspi1(HSPI); //если требуется несколько портов SPI..
Adafruit_MAX31855 thermocouple(MAXCS, &myspi1);
Adafruit_MAX31855 thermocouple_b2(MAXCS_B2, &myspi1);

//Adafruit_MAX31855 thermocouple(MAXCS, &SPI); //это аппаратный SPI (ссылка на обьект SPI) !!!

int senserror = 0; //счетчик ошибок сенсора
int kind_error = -4; //вид ошибки сенсора
double rawTemp = 0; //температура датчика без обработки (без усреднения)
double filtTemp = 0; //фильтрованное значение температуры (IIR-фильтр)
int senserror_b2 = 0;
int kind_error_b2 = -4;
double rawTemp_b2 = 0;
double filtTemp_b2 = 0;

unsigned long sensCurrentTime = 0;  //для вычисления интервалов опросов датчика
unsigned long sensLastTime = 0;  //для вычисления интервалов опросов датчика

//инициализация сенсора
void setupSensor(){

    myspi1.begin(MAXCLK, MAXDO); //если требуется несколько портов SPI..
    //SPI.begin(MAXCLK, MAXDO/*, -1, -1*/); //создаем SPI со своими пинами !!!
    //SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE0)); //скорость передачи данных 5 МГц (1 МГц умолч.)

    Serial.print("Initializing sensor...");
    if (!thermocouple.begin()) {
        Serial.println("ERROR.");
        while (1) delay(10);
    }
    Serial.println("DONE.");

    if (!thermocouple_b2.begin()) {
        Serial.println("ERROR_B2.");
        while (1) delay(10);
    }
    Serial.println("DONE_B2.");

    Serial.print("Internal Temp = "); Serial.println(thermocouple.readInternal());
    Serial.print("Internal Temp B2 = "); Serial.println(thermocouple_b2.readInternal());

    double c = thermocouple.readCelsius();
    if (isnan(c)) {
      Serial.println("Thermocouple fault(s) detected!");
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
      if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
      if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");

    } else {
      Serial.print("C = ");
      Serial.println(c);
    }

    c = thermocouple_b2.readCelsius();
    if (isnan(c)) {
      Serial.println("Thermocouple_b2 fault(s) detected!");
      uint8_t e_b2 = thermocouple_b2.readError();
      if (e_b2 & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple_b2 is open - no connections.");
      if (e_b2 & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple_b2 is short-circuited to GND.");
      if (e_b2 & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple_b2 is short-circuited to VCC.");

    } else {
      Serial.print("C_B2 = ");
      Serial.println(c);
    }

}

// получаем новые данные с термопары
// если ошибок нет - получаем новую температуру
// если ошибка, то оставляем старое значение
void updateCurrentTemperature(){
  sensCurrentTime = time_now;
  //формируем период опроса датчика
  if(sensCurrentTime - sensLastTime >= SENS_PERIOD or sensLastTime > sensCurrentTime) {

    double c = thermocouple.readCelsius(); //читаем термопару
    if (!isnan(c)) { //нет ошибки
      rawTemp = c; //запоминаем температуру без обработки для TRMPFAIL

      filtTemp += (c - filtTemp) * 0.2; //фильтр (IIR)
      currentTemp = (round(filtTemp*100))/100;
      if (senserror>=2) senserror=1;
      if (senserror==1) {senserror=0;kind_error = -4;}

    } else {
      uint8_t e = thermocouple.readError();
      if (e & MAX31855_FAULT_OPEN) kind_error = -1;
      if (e & MAX31855_FAULT_SHORT_GND) kind_error = -2;
      if (e & MAX31855_FAULT_SHORT_VCC) kind_error = -3;
      senserror++;
    }

    c = thermocouple_b2.readCelsius(); //читаем термопару
    if (!isnan(c)) { //нет ошибки
      rawTemp_b2 = c; //запоминаем температуру без обработки для TRMPFAIL

      filtTemp_b2 += (c - filtTemp_b2) * 0.2; //фильтр (IIR)
      currentTemp_b2 = (round(filtTemp_b2*100))/100;
      if (senserror_b2>=2) senserror_b2=1;
      if (senserror_b2==1) {senserror_b2=0;kind_error_b2 = -4;}

    } else {
      uint8_t e_b2 = thermocouple_b2.readError();
      if (e_b2 & MAX31855_FAULT_OPEN) kind_error_b2 = -1;
      if (e_b2 & MAX31855_FAULT_SHORT_GND) kind_error_b2 = -2;
      if (e_b2 & MAX31855_FAULT_SHORT_VCC) kind_error_b2 = -3;
      senserror_b2++;
    }


    sensLastTime = sensCurrentTime;
  }
}
