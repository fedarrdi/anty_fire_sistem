#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define STASSID "Ilko"
#define STAPSK  "...."
#define NAME "real_sensor"

ESP8266WebServer server(80);

IPAddress local_IP(192,168,1,184);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,0,0);
IPAddress primaryDNS(8,8,8,8);
IPAddress secondaryDNS(8,8,4,4);

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
  String value = " 1 ";
  if (flameModule.data[0] >= 33)
      value = " 0 ";
  server.send(200, "text/html", value);
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

void get_name()
{
  String msg = " ";
  msg += NAME;
  msg += " ";
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

  if(!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
    Serial.print("Failed");
  Serial.print(WiFi.localIP());
     
  while (WiFi.status() != WL_CONNECTED)
    delay(500);

  Serial.print(WiFi.localIP());

  if (MDNS.begin("esp8266"))
    Serial.println("MDNS responder started");

  server.on("/", handleRoot);
  server.on("/sensor_info", HTTP_GET, get_name);
  server.on("/humidity_temperature", HTTP_GET, print_humidity_temperature);
  server.on("/flame", HTTP_GET, print_flame);

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
