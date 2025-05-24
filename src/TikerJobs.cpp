#include <Arduino.h>

extern double rawTemp; //температура без фильтра 
extern volatile bool tempfail; //флаг для блокировки реле
extern double rawTemp_b2;
extern volatile bool tempfail_b2;
extern void timer_counter_decr();

double oldrawTemp = 0; //тут запоминаем с периодом 5 сек.
double oldrawTemp_b2 = 0;

unsigned long sUpTime;
unsigned long ihour;
unsigned long imin;
unsigned long isec;
unsigned long iday;


//вычислить uptime д/ч/м/с (вызывается с периодом 5 сек)
void get_uptime(){


    sUpTime+=5;  //прибавляем 5 сек
    auto n=sUpTime; //количество всех секунд
    isec = n % 60;  //остаток от деления на 60 (секунд в минуте)
    n /= 60; //количество всех минут (целая часть)  
    imin = n % 60;  //остаток от деления на 60 (минут в часе)
    n /= 60; //количество всех часов (целая часть)
    ihour = n % 24; //остаток от деления на 24 (часов в дне)
    iday = n/24; //количество всех дней (целая часть)

    //устанавливаем блокировку по резкому обвалу температуры
    if (oldrawTemp>rawTemp and abs(oldrawTemp-rawTemp)>10){tempfail = true;}
    if (oldrawTemp_b2>rawTemp_b2 and abs(oldrawTemp_b2-rawTemp_b2)>10){tempfail_b2 = true;}
    oldrawTemp = rawTemp;
    oldrawTemp_b2 = rawTemp_b2;

    timer_counter_decr();
}
