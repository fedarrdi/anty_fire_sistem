import streamlit as st
import matplotlib.pyplot as pls
import pandas as pd
import re
import time
from urllib.request import urlopen
import email_sender as es


url0 ='http://192.168.1.184/flame'
url1 = 'http://192.168.1.184/humidity_temperature'
url2 = 'http://192.168.1.184/sensor_info'

df1 = pd.DataFrame({'Temp: ':[0.0],'Hum: ':[0.0]})
df2 = pd.DataFrame({'flame': [0]})

def update_name():
    page2 = urlopen(url2)
    html2 = page2.read().decode('utf-8')
    splittext2 = re.split("[ ]", html2)
    return str(splittext2[1])

def update_flame(name):
    global df2
    page1 = urlopen(url0)
    html1 = page1.read().decode('utf-8')       
    splittext1 = re.split("[ ]", html1)
    is_flame = int(splittext1[1])
    
    if is_flame:
        msg = name 
        msg += " on fire"
        es.send_mail(msg)
    
    df2 = df2.append({'flame': is_flame}, ignore_index = True)

def update_temp_hum(name):
    global df1
    try:
        page = urlopen(url1)
        html = page.read().decode('utf-8')
        splittext = re.split('[ %C]', html)
        hum = float(splittext[2])
        temp = float(splittext[5])

        if temp > 60 or hum < 30:
            msg = temp + " " + hum
            es.send_mail(msg)

    except:
        hum = 0
        temp = 0
    df1 = df1.append({'Temp: ': temp,'Hum: ': hum}, ignore_index = True)

status_text1 = st.sidebar.empty()
status_text2 = st.sidebar.empty()
chart_weather1 = st.line_chart(df1)
chart_weather2 = st.line_chart(df2)

for i in range(0, 500):
    name = update_name()
    update_flame(name)
    update_temp_hum(name)
    chart_weather1.add_rows(df1[:][len(df1)-1: len(df1)])
    status_text1.write(df1.loc[:][::-1])
    chart_weather2.add_rows(df2[:][len(df2)-1: len(df2)])
    status_text2.write(df2.loc[:][::-1])
    time.sleep(1)
