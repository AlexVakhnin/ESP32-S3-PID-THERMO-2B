#include <Arduino.h>

#define TIMER_IND_PIN  38 //светодиод зеленый
#define TIMER_ON_PIN 37  //тумблер (инверсный вход)

extern void heat_stop_b2();

bool ind_state = false; //состояние зеленого светодиода
bool tm_on = false; //состояние таймера вкл./выкл.
bool ss_old = true; //предыдущее состояние тумблера
bool ss_old_old = true; //второе предыдущее состояние тумблера (антидребезг..)

volatile int tm_counter = 0; //счетчик таймера отключения комфорки

//настройка портов для управления таймером
void timer_setup(){
    pinMode(TIMER_IND_PIN, OUTPUT);
    digitalWrite(TIMER_IND_PIN, false);
    pinMode(TIMER_ON_PIN, INPUT_PULLUP);
}

//состояние включения тумблера "ТАЙМЕР"
bool switch_state(){
    return !digitalRead(TIMER_ON_PIN);
}

//мигаем зеленым светодиодом с частотой вызова функции (T=0.5 сек.)
void timer_ind_blink(void){
    if(tm_on){
        digitalWrite(TIMER_IND_PIN, ind_state=!ind_state); //мигаем
    } else {
        digitalWrite(TIMER_IND_PIN, false); //гасим
    }
}

//событийе по включению тумблера таймера
void event_switch_on(){
    tm_on=true;
    tm_counter = 20*60; //заряжаем таймер - 20 мин.
}

//событийе по выключению тумблера таймера
void event_switch_off(){
    tm_on=false;
    tm_counter = 0;
}

//отслеживание событий вкл./выкл. таймера (T=200 мс.)
void timer_handle(){
    bool ss = switch_state(); //текущее состояние тумблера
    if (ss==true and ss_old==false and ss_old_old==false) event_switch_on();
    if (ss==false and ss_old==true and ss_old_old==true) event_switch_off();
    ss_old_old=ss_old;ss_old=ss; //заполняем предыдущие состояния
}

//уменьшение счетчика таймера на 5 сек. (Т=5сек.)
void timer_counter_decr(){
    if(tm_on and tm_counter>0 ){
        tm_counter-=5;
        if (tm_counter<=0) {tm_counter=0; tm_on=false; heat_stop_b2();}
    }
}