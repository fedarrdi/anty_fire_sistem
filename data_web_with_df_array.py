import streamlit as st
import pandas as pd
import re
import time
from urllib.request import urlopen
import email_sender as es

status_text1 = st.sidebar.empty()
status_text2 = st.sidebar.empty()
status_text3 = st.sidebar.empty()
status_text4 = st.sidebar.empty()

url0 = 'http://192.168.1.184/flame'
url1 = 'http://192.168.1.184/humidity_temperature'
url2 = 'http://192.168.1.184/sensor_info'

url3 = 'http://192.168.1.185/test_flame'
url4 = 'http://192.168.1.185/test_h_t'
url5 = 'http://192.168.1.185/test_name'

url_array = [url0, url1, url2, url3, url4, url5]

def update_name(url):
    try:
        page = urlopen(url)
        html = page.read().decode('utf-8')
        splittext = re.split("[ ]", html)
        return str(splittext[1])
    except:
        return ""

name1 = update_name(url_array[2])
name2 = update_name(url_array[5])

df1 = pd.DataFrame({'Temp: ':[0.0],'Hum: ':[0.0]})
df2 = pd.DataFrame({'flame': [0]})
df3 = pd.DataFrame({'Temp: ':[0.0],'Hum: ':[0.0]})
df4 = pd.DataFrame({'flame': [0]})

df_array = [df1, df2, df3, df4]

st.text(name1)
chart_weather1 = st.line_chart(df1)
chart_weather2 = st.line_chart(df2)
st.text(name2)
chart_weather3 = st.line_chart(df3)
chart_weather4 = st.line_chart(df4)


def update_flame(name, url, df):
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
    df = df.append({'flame': is_flame}, ignore_index = True)
    return df

def update_temp_hum(name, url, df):
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
    df = df.append({'Temp: ': temp,'Hum: ': hum}, ignore_index = True)
    return df

while(True):
    df_array[0] = update_temp_hum(name1, url_array[1], df_array[0])
    df_array[1] = update_flame(name1, url_array[0], df_array[1])

    df_array[2] = update_temp_hum(name2, url_array[4], df_array[2])
    df_array[3] = update_flame(name2, url_array[3], df_array[3])

    chart_weather1.add_rows(df_array[0][:][len(df_array[0])-1: len(df_array[0])])
    status_text1.write(df_array[0].loc[:][::-1])
    chart_weather2.add_rows(df_array[1][:][len(df_array[1])-1: len(df_array[1])])
    status_text2.write(df_array[1].loc[:][::-1])

    chart_weather3.add_rows(df_array[2][:][len(df_array[2])-1: len(df_array[2])])
    status_text3.write(df_array[2].loc[:][::-1])
    chart_weather4.add_rows(df_array[3][:][len(df_array[3])-1: len(df_array[3])])
    status_text4.write(df_array[3].loc[:][::-1])
    time.sleep(1)