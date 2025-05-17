#include <Arduino.h>

extern double currentTemp; //температура термопары фильтрованная
extern double rawTemp; //температура без фильтра 
extern volatile bool tempfail; //флаг для блокировки реле

double oldrawTemp = 0; //тут запоминаем с периодом 5 сек.

unsigned long sUpTime;
unsigned long ihour;
unsigned long imin;
unsigned long isec;
unsigned long iday;
volatile unsigned int arrTemp[108]= {0}; //окно - 27 мин.(15*108/60)
int counter_30s = 0;


//сдвиг элементов массива влево с добавлением нового значения
void push_arr( volatile unsigned int arr[], int elem, unsigned int n ){
    for (int i=elem-1;i>0;i--){
        arr[i] = arr[i-1];
    }
    arr[0] = n;
}


//вычислить uptime д/ч/м/с (вызывается с периодом 5 сек)
void get_uptime(){

    if ( counter_30s > 0 ){counter_30s--;}
    else { 
        counter_30s = 5; //6*5=30

        //работа с массивом для графика
        int elements = sizeof(arrTemp) / sizeof(arrTemp[0]);//колич. точек X графика
        push_arr( arrTemp, elements, currentTemp*100 );//сдвиг массива arrTemp влево
    }
  


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
    oldrawTemp = rawTemp;

}
