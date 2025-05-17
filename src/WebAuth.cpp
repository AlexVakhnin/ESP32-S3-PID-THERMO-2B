#include "SPIFFS.h"
#include <ESPAsyncWebServer.h>
#include "mbedtls/md.h"

const char* www_username = "admin";
const char* www_password = "admin";

//Declaration functions
extern String processor(const String& var);
//void serverRouting();
bool is_authenticated(AsyncWebServerRequest *request);

String sha1(String payloadStr){
    const char *payload = payloadStr.c_str();
 
    int size = 20;
 
    byte shaResult[size];
 
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;
 
    const size_t payloadLength = strlen(payload);
 
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char *) payload, payloadLength);
    mbedtls_md_finish(&ctx, shaResult);
    mbedtls_md_free(&ctx);
 
    String hashStr = "";
 
    for(uint16_t i = 0; i < size; i++) {
        String hex = String(shaResult[i], HEX);
        if(hex.length() < 2) {
            hex = "0" + hex;
        }
        hashStr += hex;
    }
 
    return hashStr;
}

String getContentType(String filename) {
    if (filename.endsWith(F(".htm"))) return F("text/htmltext/html");
    else if (filename.endsWith(F(".html"))) return F("text/html; charset=utf-8");
    else if (filename.endsWith(F(".css"))) return F("text/css");
    else if (filename.endsWith(F(".js"))) return F("application/javascript");
    else if (filename.endsWith(F(".json"))) return F("application/json");
    else if (filename.endsWith(F(".png"))) return F("image/png");
    else if (filename.endsWith(F(".gif"))) return F("image/gif");
    else if (filename.endsWith(F(".jpg"))) return F("image/jpeg");
    else if (filename.endsWith(F(".jpeg"))) return F("image/jpeg");
    else if (filename.endsWith(F(".ico"))) return F("image/x-icon");
    else if (filename.endsWith(F(".xml"))) return F("text/xml");
    else if (filename.endsWith(F(".pdf"))) return F("application/x-pdf");
    else if (filename.endsWith(F(".zip"))) return F("application/x-zip");
    else if (filename.endsWith(F(".gz"))) return F("application/x-gzip");
    return F("text/plain");
}

//Формируем ответ броузеру на все запросы, которые не определены функциями httpServer.on(...)
bool handleFileRead(AsyncWebServerRequest *request, String path) {
    Serial.print(F("handleFileRead(): "));
    Serial.println(path);
 
    if (!is_authenticated(request)) { //проверка аутентификации
        Serial.print("Change path from: "+path);
        path = "/login.html"; //меняем URL !
        Serial.println(" to: "+path);
    } else {
        if (path.endsWith("/")) path += F("index.html"); // If a folder is requested, send the index file
    }
    String contentType = getContentType(path);              // Get the MIME type
      if(SPIFFS.exists(path)){     // If the file exists on FS

        Serial.print("Send to browser: ");
        Serial.println(path);

        //Отдельная обработка запросов у которых есть processor, и у каждого свой..
        /*
        if (path.equals("/index.html")){
            Serial.println("caught the /index.html, need processor..");
            AsyncWebServerResponse *response = request->beginResponse(SPIFFS, path, contentType,false, processor);
            //response->addHeader("X-Content-Type-Options", "nosniff");//не пытаться угадывать MIME (contrnt-type)
            response->addHeader("Cache-Control","no-cache");
            request->send(response);

         } else*/ 
        if (path.equals("/net.html")){
            Serial.println("caught the /net.html, need processor..");
            AsyncWebServerResponse *response = request->beginResponse(SPIFFS, path, contentType,false, processor);
            response->addHeader("Cache-Control","no-cache");
            request->send(response);

        } else { //все остальные запросы..
            AsyncWebServerResponse *response = request->beginResponse(SPIFFS, path, contentType);
            response->addHeader("Cache-Control","no-cache");
            request->send(response);
        }

        return true;
      }
    Serial.println(String(F("\tFile Not Found: ")) + path);
    return false;                     // If the file doesn't exist, return false
}

void handleNotFound(AsyncWebServerRequest *request) {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += request->url();
  message += "\nMethod: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += request->args();
  message += "\n";
 
  for (uint8_t i = 0; i < request->args(); i++) {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }
 
  request->send(404, "text/plain", message);
}

// "/login" обработчик
// принимает и обрабатывает данные из формы
void handleLogin(AsyncWebServerRequest *request) {
    Serial.println("Handle login");
    String msg;
    if (request->hasHeader("Cookie")) { //если есть в запросе куки
        // Print cookies
        Serial.print("Found cookie: ");
        String cookie = request->header("Cookie");
        Serial.println(cookie); //печатаем куки ???
    }
 
    if (request->hasArg("username") && request->hasArg("password")) { //если форма передает username и password
        Serial.print("Found parameter: ");
        //Проверяем логин-пароль
        if (request->arg("username") == String(www_username) && request->arg("password") == String(www_password)) {
            AsyncWebServerResponse *response = request->beginResponse(301); //формируем ответ - redirect (301)
 
            response->addHeader("Location", "/"); //перенаправляем в корень
            response->addHeader("Cache-Control", "no-cache");//не кешируем
            //создаем токен
            String token = sha1(String(www_username) + ":" + String(www_password) + ":" + request->client()->remoteIP().toString());
            Serial.print("Token: ");
            Serial.println(token);
            response->addHeader("Set-Cookie", "ESPSESSIONID=" + token);//токен положить в куки
 
            request->send(response); //посылаем ответ (301) броузеру
            Serial.println("Log in Successful");
            return;
        }
        // Логин-пароль не совпали
        msg = "Wrong username/password! try again.";
        Serial.println("Log in Failed");
        AsyncWebServerResponse *response = request->beginResponse(301); //ответ броузеру 301 redirect
 
        response->addHeader("Location", "/login.html?msg=" + msg);//перенаправляем снова на login.html с параметром msg
        response->addHeader("Cache-Control", "no-cache");
        request->send(response); //посылаем ответ (301) броузеру
        return;
    }
}
 
/**
 * Manage logout (simply remove correct token and redirect to login form)
 */
void handleLogout(AsyncWebServerRequest *request) {
    Serial.println("Disconnection");
    AsyncWebServerResponse *response = request->beginResponse(301); //Sends 301 redirect
 
    response->addHeader("Location", "/login.html?msg=User disconnected");
    response->addHeader("Cache-Control", "no-cache");
    response->addHeader("Set-Cookie", "ESPSESSIONID=0");
    request->send(response);
    return;
}

//Проверка присутствия правильного токена в шапке запроса от броузера
bool is_authenticated(AsyncWebServerRequest *request) {
    //Serial.println("is_authenticated(): "+request->url());//DEBUG
    if (request->hasHeader("Cookie")) {
    //    Serial.print("Found cookie: ");                   //DEBUG
        String cookie = request->header("Cookie");
    //    Serial.println(cookie);                            //DEBUG
 
        String token = sha1(String(www_username) + ":" +
                String(www_password) + ":" +
                request->client()->remoteIP().toString());
//  token = sha1(token);
 
        if (cookie.indexOf("ESPSESSIONID=" + token) != -1) {
    //        Serial.println("Authentication Successful");    //DEBUG
            return true;
        }
    }
    Serial.println("Authentication Failed");
    return false;
}
