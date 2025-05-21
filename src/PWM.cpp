#include <Arduino.h>

#define PWM_PERIOD 1000  //период PWM (общий для 2-х комфорок)
#define IND_PIN  11 //порт для индикации включения реле
#define RELAY_PIN  34 //порт управления реле
#define IND_PIN_B2  40
#define RELAY_PIN_B2  36

#define HEAT_MAX 400

extern unsigned long time_now;
extern int senserror; //счетчик ошибок термопары
extern double currentTemp;
extern double rawTemp; //температура без обработки
extern int senserror_b2;
extern double currentTemp_b2;
extern double rawTemp_b2;

bool overheat = false; //перегрев
volatile bool tempfail = false; //резкий обвал температуры (датчик физически отпал..)
bool overheat_b2 = false;
volatile bool tempfail_b2 = false;

float heatcycles = 0; //время в милисекундах верхней полки PWM (0-1000) - мощность нагрева !!!
bool pwmState = 0; //содержит текущее состояние выхода PWM (0/1)
float heatcycles_b2 = 0;
bool pwmState_b2 = 0;

unsigned long heatCurrentTime = 0;  //для вычисления интервалов PWM (общий для 2-х комфорок)
unsigned long heatLastTime = 0;  //для вычисления интервалов PWM (общий для 2-х комфорок)

void pwm_setup() {
    pinMode(RELAY_PIN, OUTPUT); //управление реле
    digitalWrite(RELAY_PIN, false);
    pinMode(IND_PIN, OUTPUT); //индикация включения реле
    digitalWrite(IND_PIN, false);

    pinMode(RELAY_PIN_B2, OUTPUT); //управление реле
    digitalWrite(RELAY_PIN_B2, false);
    pinMode(IND_PIN_B2, OUTPUT); //индикация включения реле
    digitalWrite(IND_PIN_B2, false);
}

//меняем состояние нагревателя,
//применяем аварийные блокировки
void _turnHeatElementOnOff(bool state) {

    if(senserror!=0) state = 0; //блокировка при аварии датчика !!!
    if(rawTemp>=HEAT_MAX) overheat=true;//блокировка по высокой температуре !!!
    if(overheat) state = 0;
    if(tempfail) state = 0;

    digitalWrite(RELAY_PIN, state); //реле
    digitalWrite(IND_PIN, state); //индикация
    pwmState = state;
}
//2-я комфорка
void _turnHeatElementOnOff_b2(bool state) {

    if(senserror_b2!=0) state = 0; //блокировка при аварии датчика !!!
    if(rawTemp_b2>=HEAT_MAX) overheat_b2=true;//блокировка по высокой температуре !!!
    if(overheat_b2) state = 0;
    if(tempfail_b2) state = 0;

    digitalWrite(RELAY_PIN_B2, state); //реле
    digitalWrite(IND_PIN_B2, state); //индикация
    pwmState_b2 = state;
}

//формирование PWM, вызывается из loop()
void pwm_handle() {
  heatCurrentTime = time_now;
  bool newStatus = false;
  bool shouldUpdate = false;
  bool newStatus_b2 = false;
  bool shouldUpdate_b2 = false;

  //формируем период PWM (T=1 сек.)
  if(heatCurrentTime - heatLastTime >= PWM_PERIOD or heatLastTime > heatCurrentTime) { //второй оператор предотвращает ошибки переполнения
    // начинаем новый период PWM !!!
    shouldUpdate = true; //нужно обновить
    newStatus = true;  //обновить - включить
    shouldUpdate_b2 = true;
    newStatus_b2 = true;
    //Serial.println("__/-- "+String(heatCurrentTime)+"-"+String(heatLastTime)+"="+String(heatCurrentTime - heatLastTime));
    heatLastTime = heatCurrentTime;
  }

  // формируем длительность импульса PWM
  if (heatCurrentTime - heatLastTime >= heatcycles &&  pwmState==1) {
    shouldUpdate = true; //нужно обновить
    newStatus = false;  //обновить - выключить
    //Serial.println("--\\__ "+String(heatCurrentTime)+"-"+String(heatLastTime)+"="+String(heatCurrentTime - heatLastTime));
  }
  // комфорка 2
  if (heatCurrentTime - heatLastTime >= heatcycles_b2 &&  pwmState_b2==1) {
    shouldUpdate_b2 = true; //нужно обновить
    newStatus_b2 = false;  //обновить - выключить
    //Serial.println("2--\\__ "+String(heatCurrentTime)+"-"+String(heatLastTime)+"="+String(heatCurrentTime - heatLastTime));
  }

  //меняем состояние выхода PWM (0/1)
  if(shouldUpdate){
    _turnHeatElementOnOff(newStatus);
  }
  if(shouldUpdate_b2){
    _turnHeatElementOnOff_b2(newStatus_b2);
  }
}

//Устанавливаем мощность нагрева (0-1000)
void setHeatPowerPercentage(float power) {
  if (power < 0.0) { power = 0.0; }  
  if (power > 1000.0) { power = 1000.0; }
  heatcycles = power;
}
void setHeatPowerPercentage_b2(float power) {
  if (power < 0.0) { power = 0.0; }  
  if (power > 1000.0) { power = 1000.0; }
  heatcycles_b2 = power;
}

float getHeatCycles() {
  return heatcycles;
}
