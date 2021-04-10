import streamlit as st
import pandas as pd
import re
import time
from urllib.request import urlopen
import email_sender as es


url0 = 'http://192.168.1.184/flame'
url1 = 'http://192.168.1.184/humidity_temperature'
url2 = 'http://192.168.1.184/sensor_info'

url3 = 'http://192.168.1.185/test_flame'
url4 = 'http://192.168.1.185/test_h_t'
url5 = 'http://192.168.1.185/test_name'

url_array = [url0, url1, url2, url3, url4, url5]

df1 = pd.DataFrame({'Temp: ':[0.0],'Hum: ':[0.0]})
df2 = pd.DataFrame({'flame': [0]})

def update_name(url):
    try:
        page = urlopen(url)
        html = page.read().decode('utf-8')
        splittext = re.split("[ ]", html)
        return str(splittext[1])
    except:
        return ""

def update_flame(name, url):
    global df2
    try:
        page = urlopen(url)
        html = page.read().decode('utf-8')       
        splittext = re.split("[ ]", html)
        is_flame = int(splittext[1])
        if is_flame:
            msg = """Subject: {0}

            Flame : {1}""".format(name, is_flame)
            es.send_mail(msg)
    except:
        is_flame = 0
    df2 = df2.append({'flame': is_flame}, ignore_index = True)

def update_temp_hum(name, url):
    global df1
    try:
        page = urlopen(url)
        html = page.read().decode('utf-8')
        splittext = re.split('[ %C]', html)
        
        hum = float(splittext[2])
        temp = float(splittext[5])
        if temp > 60 or hum < 30:
            msg = """Subject: {0}

            Humidity : {1}, Temperature : {2}""".format(name, hum, temp)
            es.send_mail(msg)
    except:
        temp = 0
        hum = 0    
    df1 = df1.append({'Temp: ': temp,'Hum: ': hum}, ignore_index = True)

status_text1 = st.sidebar.empty()
status_text2 = st.sidebar.empty()
chart_weather1 = st.line_chart(df1)
chart_weather2 = st.line_chart(df2)

for i in range(0, 500):
    for j in range(0, len(url_array), 3):
        name = update_name(url_array[j + 2])
        update_temp_hum(name, url_array[j + 1])
        update_flame(name,url_array[j])
        chart_weather1.add_rows(df1[:][len(df1)-1: len(df1)])
        status_text1.write(df1.loc[:][::-1])
        chart_weather2.add_rows(df2[:][len(df2)-1: len(df2)])
        status_text2.write(df2.loc[:][::-1])
        time.sleep(1)