#include <Arduino.h>
#include <Ticker.h>

#define RFR_INTERVAL 200  //для задач энкодер, дисплей (200 мс.)

extern void disp_setup();
extern void disp_refrash(); //обновить дисплей
extern void initSPIFFS();
extern void web_init();
extern void wifi_init();
extern void get_uptime(); //работа Ticker - 5 сек.
extern void pwm_setup();
extern void pwm_handle();
extern void pid_setup();
extern void pid_set_tun();
extern bool pid_compute();
extern bool pid_compute_b2();
extern void updateCurrentTemperature();
extern void setHeatPowerPercentage(float power);
extern void setHeatPowerPercentage_b2(float power);
extern void encoder_setup();
extern void encoder_handle(); //кнопка энкодера
extern int encoder_value();
extern int encoder_value_b2();
extern void setupSensor();
extern void apn_stop();
extern void timer_setup();
extern void timer_ind_blink(void);
extern void timer_handle(void);
extern String ds1;
extern String ds2;
extern double gOutputPwr; //результат вычислений PID
extern double gOutputPwr_b2;
extern double gTargetTemp; //целевая температура
extern double gTargetTemp_b2;

Ticker hTicker; //Для UpTime, графиков, защиты..

long time_now=0; //текущее время в цикле
long time_last=0; //хранит аремя для периодического события PID алгоритма

void setup() {
   Serial.begin(115200);

  if(psramInit()){ //PSRAM Initialisation
        Serial.println("\nThe PSRAM is correctly initialized");
  }else{
        Serial.println("\nPSRAM does not work");
  }
  disp_setup(); //OLED SSD1306 128X32

  initSPIFFS(); //инициализация SPIFFS
    
  hTicker.attach_ms(5000, get_uptime); //инициализация прерывания (5 sec.)

  wifi_init(); //Инициализация WIFI
  web_init();
  apn_stop(); //если APN , выключаем его кнопкой т.к. влияет на сенсор...
  pwm_setup();
  pid_setup();
  encoder_setup();
  setupSensor();
  timer_setup();

  delay(100);
  Serial.printf("Free heap after create objects:\t%d \r\n", ESP.getFreeHeap());

  time_now=millis();
  time_last=time_now;
}

//float mapvalue  = 0;    //DEBUG
//float mapvalue_b2  = 0; //DEBUG

void loop() {

  time_now=millis();
  updateCurrentTemperature(); //обновление текущей температуры с термопары (150 мс.)

  if(abs(time_now-time_last)>=RFR_INTERVAL or time_last > time_now) { //обработка задач для T=200 мс.
      gTargetTemp = encoder_value(); //данные с энкодера
      gTargetTemp_b2 = encoder_value_b2(); //данные с энкодера 2

      //mapvalue = map(gTargetTemp,100,300,0,1000); setHeatPowerPercentage(mapvalue);             //DEBUG
      //mapvalue_b2 = map(gTargetTemp_b2,100,300,0,1000); setHeatPowerPercentage_b2(mapvalue_b2); //DEBUG

      disp_refrash();
      pid_set_tun();
      timer_handle();
      time_last=time_now;
  }


  /**/
  if (pid_compute()){ //вычисляем..если результат PID готов.. 
      setHeatPowerPercentage(gOutputPwr);  //задаем значение для PWM (0-1000)
  }
  if (pid_compute_b2()){ //вычисляем..если результат PID готов.. 
      setHeatPowerPercentage_b2(gOutputPwr_b2);  //задаем значение для PWM (0-1000)
      timer_ind_blink();//тут частота 0.5 сек. - используем ее..
  }
  /**/

  pwm_handle(); //обработчик PWM T=1000 
  encoder_handle(); //обработчик кнопки энкодера
}
