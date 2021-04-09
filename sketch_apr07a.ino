#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define STASSID "Ilko"
#define STAPSK  "..."

ESP8266WebServer server(80);

IPAddress local_IP(192,168,1,185);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,0,0);
IPAddress primaryDNS(8,8,8,8);
IPAddress secondaryDNS(8,8,4,4);

void test_name()
{
  String msg = " ";
  msg += "mitko_radi";
  msg += " ";
  server.send(200, "text/html", msg);
}

void test_h_t()
{
  String msg = "<br>";
  msg += "Humdity = 20.30";
  msg += "%</br><br>";
  msg += "Temperature = 55.00";
  msg += "C</br>";
  server.send(200, "text/html", msg);
}

void test_flame()
{
  String value = " 0 ";
  server.send(200, "text/html", value);
}

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

  server.on("/test_name", HTTP_GET,test_name);
  server.on("/test_h_t", HTTP_GET,test_h_t);
  server.on("/test_flame", HTTP_GET, test_flame);

  server.begin();
}

void setup() {
  Serial.begin(9600);
  setup_server();
}

void loop() {
 server.handleClient();
}
