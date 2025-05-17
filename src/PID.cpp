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

// создаем экземпляр ESPPID, 
// в конструктор передаем ссылки на переменные откуда брать значения и куда передавать результат
// gTargetTemp, currentTemp, gOutputPwr
// так же задаем коэффициенты для PID -> gP, gI, gD
PID ESPPID(&currentTemp, &gOutputPwr, &gTargetTemp, gP, gI, gD, DIRECT);

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
}

//вычисляет PID
bool pid_compute(){
    return ESPPID.Compute(); //сама рещает, нужно ли вычислять, если вычисляет то true
}

//задаем коэффициенты, период, выходной диапазон, режим работы
void pid_setup(){

  // start PID
  ESPPID.SetTunings(gP, gI, gD);
  ESPPID.SetSampleTime(PID_INTERVAL); //задаем период для вычислений PID
  ESPPID.SetOutputLimits(0, 1000);
  ESPPID.SetMode(AUTOMATIC);
}
