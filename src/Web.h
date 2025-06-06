#include "SPIFFS.h"
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "ArduinoJson.h"

String processor(const String& var); //internal function

extern void writeFile(fs::FS &fs, const char * path, const char * message);
extern String readFile(fs::FS &fs, const char * path);

extern unsigned long sUpTime;
extern unsigned long ihour;
extern unsigned long imin;
extern unsigned long isec;
extern unsigned long iday;

extern double gTargetTemp; //целевая температура
extern double currentTemp; //текущая температура
extern double gOutputPwr; //процент мощности на нагреватель (0-1000)
extern bool overShootMode; //агрессивный режим
extern double gTargetTemp_b2;
extern double currentTemp_b2;
extern double gOutputPwr_b2;
extern bool overShootMode_b2;
//extern volatile unsigned int arrTemp[108];

extern void handleLogin(AsyncWebServerRequest *request);
extern void handleLogout(AsyncWebServerRequest *request);
extern void handleNotFound(AsyncWebServerRequest *request);
extern bool handleFileRead(AsyncWebServerRequest *request, String path);
extern bool is_authenticated(AsyncWebServerRequest *request);

//для графиков
extern int get_kp();
extern int get_ki();
extern int get_kd();

extern int get_kp_b2();
extern int get_ki_b2();
extern int get_kd_b2();
