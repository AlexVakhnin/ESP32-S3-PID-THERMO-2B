
#include "Web.h"  //инклуды и внешние функции
#include "ArduinoJson.h"



// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Search for parameter in HTTP POST request
const char* PARAM_SSID1 = "ssid1";
const char* PARAM_PASS1 = "pass1";
const char* PARAM_SSID2 = "ssid2";
const char* PARAM_PASS2 = "pass2";
const char* PARAM_RESET = "reset";

const char* ssid1Path = "/ssid1.txt"; //SPIFFS file name
const char* pass1Path = "/pass1.txt"; //SPIFFS file name
const char* ssid2Path = "/ssid2.txt"; //SPIFFS file name
const char* pass2Path = "/pass2.txt"; //SPIFFS file name

const char* http_username = "admin";
const char* http_password = "admin";

//обработка текста переданного клиентом заросом htttp POST
void onConnectBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  if (!is_authenticated(request)) {Serial.println("!!! blocking hackers !!!");return;} //если нет аутентификации, выход..
  StaticJsonDocument<200> doc; //резервируем место в памяти

  auto error = deserializeJson(doc, data);  //разбор json структуры
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));  //ошибка разметки
    Serial.println(error.c_str());
    return;
  }
  JsonObject root = doc.as<JsonObject>(); //у нас не массив , а обьект json

  for (JsonPair keyValue : root) { //проход о всем парам обьекта json

    if (keyValue.key() == PARAM_SSID1){
      String val = keyValue.value().as<const char*>();//значение
      Serial.print("SSID1 set to: ");
      Serial.println(val);
      writeFile(SPIFFS, ssid1Path, val.c_str());    
    }
    if (keyValue.key() == PARAM_PASS1){
      String val = keyValue.value().as<const char*>();
      Serial.print("PASS1 set to: ");
      Serial.println(val);
      writeFile(SPIFFS, pass1Path, val.c_str());    
    }
    if (keyValue.key() == PARAM_SSID2){
      String val = keyValue.value().as<const char*>();//значение
      Serial.print("SSID2 set to: ");
      Serial.println(val);
      writeFile(SPIFFS, ssid2Path, val.c_str());    
    }
    if (keyValue.key() == PARAM_PASS2){
      String val = keyValue.value().as<const char*>();
      Serial.print("PASS2 set to: ");
      Serial.println(val);
      writeFile(SPIFFS, pass2Path, val.c_str());    
    }
    if (keyValue.key() == PARAM_RESET){
      String val = keyValue.value().as<const char*>();
      Serial.print("RESET to: ");
      Serial.println(val);
      if(val.equals("yes")){delay(3000);ESP.restart();};    
    }

  }

}
 
void web_init(){
/*
  // корень с проверкой аутентификации (так тоже работает нормально !!!)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){    
    if (is_authenticated(request)) { 
      request->send(SPIFFS, "/index.html", String(), false, processor);
    } else {
      request->send(SPIFFS, "/login.html", "text/html");
    }
  });
*/
//POST
//---------------------------------------------------------------------------

server.on("/posts", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain","POSTED SUCCESSFULLY");
}, NULL, onConnectBody);

//---------------------------------------------------------------------------

    // Точки входа для страниц, где нет аутентификации
    server.on("/login", HTTP_POST, handleLogin);
    server.on("/logout", HTTP_GET, handleLogout);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.html", String(), false, processor);
    });
    server.on("/graph.html", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/graph.html", String(), false, processor);
    });
    server.on("/pid.html", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/pid.html", String(), false, processor);
    });
  

    // Все картинки кэшируем без проверки
    server.on("/signin.png", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/signin.png", "image/png");
    response->addHeader("Cache-Control","max-age=31536000, immutable"); 
    request->send(response);
    });
    
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/favicon.ico", "image/x-icon");
    response->addHeader("X-Content-Type-Options", "nosniff");//не пытаться угадывать MIME (contrnt-type)
    response->addHeader("Cache-Control","max-age=31536000, immutable");
    request->send(response);
    });
    
    server.on("/network.png", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/network.png", "image/png");
    response->addHeader("Cache-Control","max-age=31536000, immutable"); 
    request->send(response);
    });
    server.on("/logout.png", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/logout.png", "image/png");
    response->addHeader("Cache-Control","max-age=31536000, immutable"); 
    request->send(response);
    });
    server.on("/elegantota.png", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/elegantota.png", "image/png");
    response->addHeader("Cache-Control","max-age=31536000, immutable");
    request->send(response);
    });

      // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to load Chart.min.js file
  server.on("/Chart.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/Chart.min.js", "text/javascript");
    response->addHeader("Cache-Control","max-age=3600"); /*very important !!!!!*/
    request->send(response);
  });


  // Это ответ сервера на AJAX запрос из WEB страницы"
  // с проверкой аутентификации
  server.on("/jsonstate", HTTP_GET, [](AsyncWebServerRequest *request){
    if (is_authenticated(request)) { 
      AsyncResponseStream *response = request->beginResponseStream("application/json; charset=utf-8"); 
      response->addHeader("Cache-Control", "no-cache");//не кешируем 
      response->addHeader("X-Content-Type-Options", "nosniff");//не пытаться угадывать MIME (contrnt-type)

      const int capacity = JSON_OBJECT_SIZE(6);//Количество живых параметров JSON
      StaticJsonDocument<capacity> doc;
      doc["rssi"] = WiFi.RSSI();
      doc["freemem"] = ESP.getFreeHeap();  //или String().c_str();???????
      doc["iday"] = iday;
      doc["ihour"] = ihour;
      doc["imin"] = imin;
      doc["isec"] = isec;
      serializeJson(doc, *response);
      request->send(response);
    } else {
      request->send(200, "text/plain","Authentication Error"); //ответ, т.к вызываем в фоне
    }
  });

    // график №1
    server.on("/jsong0", HTTP_GET, [](AsyncWebServerRequest *request){
      //switch_flag = 0;
      AsyncResponseStream *response = request->beginResponseStream("application/json");    
      const int capacity = JSON_OBJECT_SIZE(108);//Количество живых параметров = 108
      StaticJsonDocument<capacity> doc;
          for (int i = 107; i >= 0; i--) {  //график №1 в JSON
            doc.add( arrTemp[i] ); //simply array !!!
          }
      serializeJson(doc, *response);
      request->send(response);    
    });

    // текущие данные для графика №1
    server.on("/jsond0", HTTP_GET, [](AsyncWebServerRequest *request){
      //switch_flag = 0;
      AsyncResponseStream *response = request->beginResponseStream("application/json");    
      const int capacity = JSON_OBJECT_SIZE(3);//Количество живых параметров = 108
      StaticJsonDocument<capacity> doc;
          //for (int i = 2; i >= 0; i--) {  //график №1 в JSON
          //  doc.add( arrTemp[i] ); //simply array !!!
          //}
          doc.add( gTargetTemp ); //цель
          doc.add( currentTemp*100 ); //текущее значение
          doc.add( gOutputPwr ); //воздействие
      serializeJson(doc, *response);
      request->send(response);    
    });
  

    //Точка входа для всех защищенных страниц, для которых нет директив httpServer.on(...)
    server.onNotFound([](AsyncWebServerRequest *request) {   //клиентский запрос URL
                Serial.println("onNotFound(): "+request->url());
                if (!handleFileRead(request, request->url())){   // если на FS такого файла нет...
                    handleNotFound(request);    // выдаем ответ 404 (Not Found) error
                }                               // иначе зарос обработает handleFileRead()
            });

  //cache-control для файлов..
  //server.serveStatic("/index.html", SPIFFS, "/index.html").setCacheControl("no-cache");//, "no-cache");

  AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
  server.begin();  
}

//Динамическая замена всех параметров на WEB странице "/"
String processor(const String& var){
  if (var == "RSSI"){
      return String(WiFi.RSSI());
  }
  else if(var == "ssid1"){  //что менять
      return readFile(SPIFFS, ssid1Path);//чем менять
  }
  else if (var == "pass1"){
      return readFile(SPIFFS, pass1Path);
  }
  else if(var == "ssid2"){  //что менять
      return readFile(SPIFFS, ssid2Path);//чем менять
  }
  else if (var == "pass2"){
      return readFile(SPIFFS, pass2Path);
  }

 return String();
}
