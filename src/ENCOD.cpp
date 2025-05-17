#include <Arduino.h>

#define CLK_PIN 6
#define DT_PIN 7
#define SW_PIN 8

extern volatile bool tempfail; //флаг для блокировки реле по резкому падению температуры
extern bool overheat; //флаг для блокировки реле по перегреву
extern boolean flag_apn;

volatile int counter = 0;
int lastStateCLK;

//Обработка прерывания
void IRAM_ATTR rotary_encoder() {
    int currentStateCLK = digitalRead(CLK_PIN);
    if (currentStateCLK != lastStateCLK) {
        if (digitalRead(DT_PIN) != currentStateCLK) {
            counter++;
            if (counter>380){counter=380;} //ограничение вверх
            else if (counter==1){counter=150;} //пропуск (1-149) при увеличении
        } else {
            counter--;
            if (counter<0){counter=0;} //ограничение вниз
            else if (counter==149){counter=0;} //пропуск (1-149) при уменьшении
        }
        //Serial.print("Position: ");
        //Serial.println(counter);
    }
    lastStateCLK = currentStateCLK;
}

//начальные установки
void encoder_setup() {
    pinMode(CLK_PIN, INPUT);
    pinMode(DT_PIN, INPUT);
    pinMode(SW_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLK_PIN), rotary_encoder, CHANGE);
}

// обработчик в цикле LOOP
void encoder_handle() {
    if (digitalRead(SW_PIN) == LOW) {
        //Serial.println("Button Pressed");
        if(counter != 0){counter=0;}
        tempfail = false;
        overheat = false;
    }
}

int encoder_value(){
    return counter;
}