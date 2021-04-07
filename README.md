# TTTGC Mk1
TTTGC Mk1 is a plastic box with a circuit and two sensors inside: one for detecting humidity and temperature levels and one that, using an IR camera, returns an analog value which, the closer it is to 0, the greater the fire. The circuit is a NodeMCU v1.0 using a popular WiFi module called ESP8266 which acts as a station for the data from the sensors and it can be easily accessed from anywhere that has an internet connection. The software is made so that you can add, with relative ease, as many modules as you want, as long as there's enough memory. 
## Usage
All data from the device can be accessed, from the web using (<insert public IP/domain here>), which in this day and age, can be done from almost anywhere in the world. Last year was a clear indicator, with the Australian fires, that fire hazards are extremely dangerous and hard to control after they spread. Keeping this in mind, we decided that a system which is easy and cheap to manufacture, distribute and upgrade can be used by fire rangers to keep track of high risk areas(using the temperature and humidity modules) and should a fire happen(checked by the IR camera module) they can act quickly and stop the fires as soon as possible.
## Software flowchart
See s_chart.pdf
## Hardware flowchart
See h_chart.pdf
## Networking flowchart
See n_chart.pdf
## User friendly interface
Using python script we display the data via a graph table.Making it easier for clients to use.
## Links for info
  http://192.168.1.184/flame  
  http://192.168.1.184/humidity_temperature
## Case
The case is made out of recycled plastic that can be decomposed for 10-20 years at most.
## Libraries
We are using the ESP8266 package from the Arduino IDE.
```c
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
```  

```py
import streamlit as st
import matplotlib.pyplot as pls
import pandas as pd
import re
import time
from urllib.request import urlopen
```
Additional Board Manager:
http://arduino.esp8266.com/stable/package_esp8266com_index.json
## Future Plans
Add more modules for more accurate data.  
Add standalone app for easier usage.  
Upgrade the user client.  
