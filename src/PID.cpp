#include <Arduino.h>
#include <PID_v1.h>

#define PID_INTERVAL 500//1000//200  //(в милисекундах) период  для PID алгоритма !!!
//нормальная (хорошие рабочие 75, 0.7, 1200 gap=10 65, 0.0, 0.0 !!!)
#define S_P 75//91.0
#define S_I 0.7//0.26
#define S_D 1200//7950.0
//агрессивная
#define S_aP 65//85//50//100.0
#define S_aI 0.0
#define S_aD 0.0
#define S_TBAND 10//2//1.5  //Это GAP !!!

#define S_TSET 32.0 //целевая начальная
#define S_TSET_B2 32.0

extern unsigned long time_now;
extern unsigned long time_last;

double currentTemp = 0; //текущая температура
double gTargetTemp=S_TSET; //целевая температура
double gOutputPwr=0.0; //процент мощности на нагреватель (0-1000)
double currentTemp_b2 = 0;
double gTargetTemp_b2=S_TSET_B2;
double gOutputPwr_b2=0.0;

double gOvershoot=S_TBAND; //Это Gap..
double gP = S_P, gI = S_I, gD = S_D; // 91.0, 0.26, 7950.0 - STD
double gaP = S_aP, gaI = S_aI, gaD = S_aD; // 100.0, 0.0, 0.0 - STD

bool overShootMode = false; //если true - агрессивный PID..
bool overShootMode_b2 = false; //если true - агрессивный PID..

// создаем экземпляр ESPPID, 
// в конструктор передаем ссылки на переменные откуда брать значения и куда передавать результат
// gTargetTemp, currentTemp, gOutputPwr
// так же задаем коэффициенты для PID -> gP, gI, gD
PID ESPPID(&currentTemp, &gOutputPwr, &gTargetTemp, gP, gI, gD, DIRECT);
PID ESPPID_B2(&currentTemp_b2, &gOutputPwr_b2, &gTargetTemp_b2, gP, gI, gD, DIRECT);

//новые коэффициенты для PID  
void pid_set_tun(){

    double gap = abs(gTargetTemp-currentTemp); //distance away from setpoint
    if( !overShootMode && gap >= gOvershoot ) {     //ощибка по температуре > Gap   
        ESPPID.SetTunings(gaP, gaI, gaD); // 100.0, 0.0, 0.0 (форсаж)
        overShootMode=true;
    }
    else if( overShootMode && gap < gOvershoot ) {  //ощибка по температуре < Gap
        ESPPID.SetTunings(gP,gI,gD); // 91.0, 0.26, 7950.0 (норма)
        overShootMode=false;
    }

    double gap_b2 = abs(gTargetTemp_b2-currentTemp_b2); //distance away from setpoint
    if( !overShootMode_b2 && gap_b2 >= gOvershoot ) {     //ощибка по температуре > Gap   
        ESPPID_B2.SetTunings(gaP, gaI, gaD); // 100.0, 0.0, 0.0 (форсаж)
        overShootMode_b2=true;
    }
    else if( overShootMode_b2 && gap_b2 < gOvershoot ) {  //ощибка по температуре < Gap
        ESPPID_B2.SetTunings(gP,gI,gD); // 91.0, 0.26, 7950.0 (норма)
        overShootMode_b2=false;
    }
}

//вычисляет PID
bool pid_compute(){
    return ESPPID.Compute(); //сама рещает, нужно ли вычислять, если вычисляет то true
}
bool pid_compute_b2(){
    return ESPPID_B2.Compute(); //сама рещает, нужно ли вычислять, если вычисляет то true
}

//задаем коэффициенты, период, выходной диапазон, режим работы
void pid_setup(){
  ESPPID.SetTunings(gP, gI, gD);
  ESPPID.SetSampleTime(PID_INTERVAL); //задаем период для вычислений PID
  ESPPID.SetOutputLimits(0, 1000);
  ESPPID.SetMode(AUTOMATIC);

  ESPPID_B2.SetTunings(gP, gI, gD);
  ESPPID_B2.SetSampleTime(PID_INTERVAL); //задаем период для вычислений PID
  ESPPID_B2.SetOutputLimits(0, 1000);
  ESPPID_B2.SetMode(AUTOMATIC);
}
