#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define STASSID "Ndd_wrt"
#define STAPSK  "tania123"

ESP8266WebServer server(80);

struct Module
{
  byte size_;
  byte pin;
  byte *data;
} humidity_temperature_module, flame_module;

void fill_flame_data_module()
{
  flame_module.data[0] = analogRead(flame_module.pin);
}

void print_flame_data_module()
{
  String msg = "Yes";
  if (flame_module.data[0] >= 33)
    msg = "No";
  server.send(200, "text/html", msg);
}

byte read_data_humidity_temperature(byte pin)
{
  byte result = 0;
  for (byte i = 0; i < 8; i++)
  {
    while (digitalRead(pin) == LOW);
      delayMicroseconds(30); 
      if (digitalRead(pin) == HIGH)
        result |= (1 << (8 - i)); 
    while (digitalRead(pin) == HIGH);
  }
  return result;
}

void fill_data_humidity_temperature()
{
  digitalWrite(humidity_temperature_module.pin, LOW);
  delay(30);
  digitalWrite(humidity_temperature_module.pin, HIGH);
  delayMicroseconds(40);
  pinMode(humidity_temperature_module.pin, INPUT);
  while (digitalRead(humidity_temperature_module.pin) == HIGH);
  delayMicroseconds(80);
  if (digitalRead(humidity_temperature_module.pin) == LOW)
    delayMicroseconds(80);
  for (int i = 0; i < humidity_temperature_module.size_; i++)
    humidity_temperature_module.data[i] = read_data_humidity_temperature(humidity_temperature_module.pin);
  pinMode(humidity_temperature_module.pin, OUTPUT);
  digitalWrite(humidity_temperature_module.pin, HIGH);
}

void print_humidity_temperature()
{
  String msg = "<br>";
  msg += "Humdity = ";
  msg += String(humidity_temperature_module.data[0]);
  msg += '.';
  msg += String(humidity_temperature_module.data[1]);
  msg += "%</br><br>";
  msg += "Temperature = ";
  msg += String(humidity_temperature_module.data[2]);
  msg += '.';
  msg += String(humidity_temperature_module.data[3]);
  msg += "C</br>";
  server.send(200, "text/html", msg);
}

void create_module(struct Module *module, byte size_, byte pin)
{
  module->size_ = size_;
  module->pin = pin;
  module->data = (byte*)malloc(sizeof(byte) * module->size_);
  pinMode(module->pin, OUTPUT);
}

//----functions to handle pages --------------------------------------

void handleRoot()
{
  server.send(200, "text/plain", "hello from esp8266!");
}


void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";

  server.send(404, "text/html", message);
}
//--------------------------------------------------------------------

void setup_server()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED)
    delay(500);
    Serial.print(".");

  Serial.print(WiFi.localIP());

  
  if (MDNS.begin("esp8266"))
    Serial.println("MDNS responder started");

  server.on("/", handleRoot);
  server.on("/humidity_temperature", HTTP_GET, print_humidity_temperature);
  server.on("/flame_data_module", HTTP_GET, print_flame_data_module);

  server.onNotFound(handleNotFound);
  server.begin();
}

void setup()
{
  Serial.begin(9600);
  setup_server();
  create_module(&humidity_temperature_module, 4, 5);
  create_module(&flame_module, 1, A0);
}

void loop()
{
  server.handleClient();
  fill_data_humidity_temperature();
  fill_flame_data_module();
  delay(3600);
}
