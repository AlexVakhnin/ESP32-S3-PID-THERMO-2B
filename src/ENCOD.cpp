#include <Arduino.h>

#define CLK_PIN 6 //конденсатор 0.1 мкф на землю ОБЯЗАТЕЛЬНО !!!
#define DT_PIN 7
#define SW_PIN 8

#define CLK_PIN_B2 5 //конденсатор 0.1 мкф на землю ОБЯЗАТЕЛЬНО !!!
#define DT_PIN_B2 4
#define SW_PIN_B2 2

#define STEP 5 //шаг энкодера

extern volatile bool tempfail; //флаг для блокировки реле по резкому падению температуры
extern bool overheat; //флаг для блокировки реле по перегреву
extern volatile bool tempfail_b2;
extern bool overheat_b2;

volatile int counter = 0;
int lastStateCLK;

volatile int counter_b2 = 0;
int lastStateCLK_b2;

//Обработка прерывания
void IRAM_ATTR rotary_encoder() {
    int currentStateCLK = digitalRead(CLK_PIN);
    if (currentStateCLK != lastStateCLK) {
        if (digitalRead(DT_PIN) != currentStateCLK) {
            counter+=STEP;//шаг вверх
            if (counter>380){counter=380;} //ограничение вверх
            else if (counter==STEP){counter=100;} //пропуск (1-99) при увеличении
        } else {
            counter-=STEP;//шаг вниз
            if (counter<0){counter=0;} //ограничение вниз
            else if (counter==100-STEP){counter=0;} //пропуск (1-99) при уменьшении
        }
    }
    lastStateCLK = currentStateCLK;
}
// обработка прерывания для энкодера 2
void IRAM_ATTR rotary_encoder_b2() {
    int currentStateCLK = digitalRead(CLK_PIN_B2);
    if (currentStateCLK != lastStateCLK_b2) {
        if (digitalRead(DT_PIN_B2) != currentStateCLK) {
            counter_b2+=STEP;//шаг вверх
            if (counter_b2>360){counter_b2=360;} //ограничение вверх
            else if (counter_b2==STEP){counter_b2=100;} //пропуск (1-99) при увеличении
        } else {
            counter_b2-=STEP;//шаг вниз
            if (counter_b2<0){counter_b2=0;} //ограничение вниз
            else if (counter_b2==100-STEP){counter_b2=0;} //пропуск (1-99) при уменьшении
        }
    }
    lastStateCLK_b2 = currentStateCLK;
}

//начальные установки для 2-х энкодеров
void encoder_setup() {
    pinMode(CLK_PIN, INPUT);
    pinMode(DT_PIN, INPUT);
    pinMode(SW_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLK_PIN), rotary_encoder, CHANGE);

    pinMode(CLK_PIN_B2, INPUT);
    pinMode(DT_PIN_B2, INPUT);
    pinMode(SW_PIN_B2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLK_PIN_B2), rotary_encoder_b2, CHANGE);
}

// обработчик в цикле LOOP для кнопок 2-х энкодеров
void encoder_handle() {
    if (digitalRead(SW_PIN) == LOW) {
        //Serial.println("Button Pressed");
        if(counter != 0){counter=0;}
        tempfail = false;
        overheat = false;
    }

    if (digitalRead(SW_PIN_B2) == LOW) {
        //Serial.println("Button_b2 Pressed");
        if(counter_b2 != 0){counter_b2=0;}
        tempfail_b2 = false;
        overheat_b2 = false;
    }
}

//сброс целевой температуры
void heat_stop_b2(){
    counter_b2 = 0;
}

int encoder_value(){
    return counter;
}
int encoder_value_b2(){
    return counter_b2;
}
