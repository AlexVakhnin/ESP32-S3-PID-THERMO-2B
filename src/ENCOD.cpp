#include <Arduino.h>

#define CLK_PIN 6 //конденсатор 0.1 мкф на землю ОБЯЗАТЕЛЬНО !!!
#define DT_PIN 7
#define SW_PIN 8

#define CLK_PIN_B2 5
#define DT_PIN_B2 4
#define SW_PIN_B2 2

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

void IRAM_ATTR rotary_encoder_b2() {
    int currentStateCLK = digitalRead(CLK_PIN_B2);
    if (currentStateCLK != lastStateCLK_b2) {
        if (digitalRead(DT_PIN_B2) != currentStateCLK) {
            counter_b2++;
            if (counter_b2>380){counter_b2=380;} //ограничение вверх
            else if (counter_b2==1){counter_b2=100;} //пропуск (1-99) при увеличении
        } else {
            counter_b2--;
            if (counter_b2<0){counter_b2=0;} //ограничение вниз
            else if (counter_b2==99){counter_b2=0;} //пропуск (1-99) при уменьшении
        }
        //Serial.print("Position: ");
        //Serial.println(counter);
    }
    lastStateCLK_b2 = currentStateCLK;
}

//начальные установки
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

// обработчик в цикле LOOP
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

int encoder_value(){
    return counter;
}