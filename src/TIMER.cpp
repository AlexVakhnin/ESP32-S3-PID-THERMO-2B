#include <Arduino.h>

#define TIMER_IND_PIN  38 //светодиод зеленый
#define TIMER_ON_PIN 37  //тумблер (инверсный вход)

bool t_state = false;

//настройка портов для управления таймером
void timer_setup(){
    pinMode(TIMER_IND_PIN, OUTPUT);
    digitalWrite(TIMER_IND_PIN, false);
    pinMode(TIMER_ON_PIN, INPUT_PULLUP);
}

//состояние включения тумблера "ТАЙМЕР"
bool timer_on_state(){
    return !digitalRead(TIMER_ON_PIN);
}

//мигаем зеленым светодиодом с частотой вызова функции
void timer_ind_blink(void){
    if(timer_on_state()){
        digitalWrite(TIMER_IND_PIN, t_state=!t_state);
    } else {
        digitalWrite(TIMER_IND_PIN, false);
    }
}