#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Configuração da tua Rede Wi-Fi
const char *ssid = "IOTBH";
const char *password = "IOT2017@";

// Pino do Atuador
const int atuatorPin = 4;
// Pino do Led Interno
const int ledPin = 2;
// Webserver interno
AsyncWebServer server(80);

// Sensor DHT
DHT dht(5, DHT11);

String readDHTTemperature()
{
  float t = dht.readTemperature();
  return isnan(t) ? "--" : String(t);
}

String readDHTHumidity()
{
  float h = dht.readHumidity();
  return isnan(h) ? "--" : String(h);
}

String stateToString(bool state)
{
  if (state)
  {
    return F("LIGADO");
  }
  else
  {
    return F("DESLIGADO");
  }
}
// Html Processor
String htmlProcessor(const String &var)
{
  if (var == "RELAY_STATE")
  {
    return stateToString(!digitalRead(atuatorPin));
  }
  else if (var == "LED_STATE")
  {
    return stateToString(digitalRead(ledPin));
  }
  else if (var == "TEMPERATURE")
  {
    return readDHTTemperature();
  }
  else if (var == "HUMIDITY")
  {
    return readDHTHumidity();
  }
  return F("");
}
void setupWebServer()
{
  // Endpoint /
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, htmlProcessor);
  });

  // Endpoint para devolver o CSS
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Endpoint devolver o logo
  server.serveStatic("/logo.png", SPIFFS, "/logo.png").setCacheControl("max-age=600");

  // Endpoint para alterar o estado do atuador
  server.on("/toggle_relay", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(atuatorPin, !digitalRead(atuatorPin));
    request->send(SPIFFS, "/index.html", String(), false, htmlProcessor);
  });
  // Endpoint para alterar o estado do atuador
  server.on("/toggle_led", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    request->send(SPIFFS, "/index.html", String(), false, htmlProcessor);
  });
  // Endpoint para devolver temperatura
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  // Endpoint para devolver humidade
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  // Inicia o Webserver Interno
  server.begin();
}

void setup()
{
  //Apenas para Debug
  Serial.begin(115200);

  //Iniciar o sensor de temperatura
  dht.begin();
  //Configurar os pinos como Saida
  pinMode(atuatorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Inicializar o armanezamento interno do ESP32
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS ERROR");
    return;
  }

  // Ligar ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Imprime na no Monitor Serial o IP do ESP32
  Serial.println(WiFi.localIP());

  setupWebServer();
}

void loop()
{
}
