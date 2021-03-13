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
}humidityTemperatureModule, flameModule;

void fill_flame()
{
  flameModule.data[0] = analogRead(flameModule.pin);
}

void print_flame()
{
  String msg = "Yes";
  if (flameModule.data[0] >= 33)
    msg = "No";
  server.send(200, "text/html", msg);
}

byte read_humidity_temperature(byte pin)
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

void fill_humidity_temperature()
{
  digitalWrite(humidityTemperatureModule.pin, LOW);
  delay(30);
  digitalWrite(humidityTemperatureModule.pin, HIGH);
  delayMicroseconds(40);
  pinMode(humidityTemperatureModule.pin, INPUT);
  while (digitalRead(humidityTemperatureModule.pin) == HIGH);
  delayMicroseconds(80);
  if (digitalRead(humidityTemperatureModule.pin) == LOW)
    delayMicroseconds(80);
  for (int i = 0; i < humidityTemperatureModule.size_; i++)
    humidityTemperatureModule.data[i] = read_humidity_temperature(humidityTemperatureModule.pin);
  pinMode(humidityTemperatureModule.pin, OUTPUT);
  digitalWrite(humidityTemperatureModule.pin, HIGH);
}

void print_humidity_temperature()
{
  String msg = "<br>";
  msg += "Humdity = ";
  msg += String(humidityTemperatureModule.data[0]);
  msg += '.';
  msg += String(humidityTemperatureModule.data[1]);
  msg += "%</br><br>";
  msg += "Temperature = ";
  msg += String(humidityTemperatureModule.data[2]);
  msg += '.';
  msg += String(humidityTemperatureModule.data[3]);
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
  server.on("/flame_data_module", HTTP_GET, print_flame);

  server.onNotFound(handleNotFound);
  server.begin();
}

void setup()
{
  Serial.begin(9600);
  setup_server();
  create_module(&humidityTemperatureModule, 4, 5);
  create_module(&flameModule, 1, A0);
}

void loop()
{
  server.handleClient();
  fill_humidity_temperature();
  fill_flame();
  delay(3600);
}
