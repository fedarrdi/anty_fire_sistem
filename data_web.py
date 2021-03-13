import streamlit as st
import matplotlib.pyplot as pls
import pandas as pd
import re
import sys
import time
from urllib.request import urlopen

url0 ='http://192.168.1.184/flame'
url1 = 'http://192.168.1.184/humidity_temperature'

df1 = pd.DataFrame({'Temp: ':[0.0],'Hum: ':[0.0]})
df2 = pd.DataFrame({'flame': [False]})
@st.cache
def update_flame():
    global df2
    try:
        page1 = urlopen(url0)
        html1 = page.read().decode('utf-8')
        status_text2.write(html1)
        st.write(html1)
        splittext1 = re.split("[ ]", html1)
        is_flame = bool(splittext1[1])
    except:
        is_flame = False
    df2 = df2.append({'flame': is_flame}, ignore_index = True)

def update_temp_hum():
    global df1
    try:
        page = urlopen(url1)
        html = page.read().decode('utf-8')
        status_text1.write(html)
        splittext = re.split('[ %C]', html)
        hum = float(splittext[2])
        temp = float(splittext[5])
    except:
        hum = 0
        temp = 0
    df1 = df1.append({'Temp: ': temp,'Hum: ': hum}, ignore_index = True)

status_text1 = st.sidebar.empty()
status_text2 = st.sidebar.empty()
chart_weather1 = st.line_chart(df1)
chart_weather2 = st.line_chart(df2)

for i in range(0, 500):
    update_flame()
    update_temp_hum()
    chart_weather1.add_rows(df1[:][len(df1)-1: len(df1)])
    status_text1.write(df1.loc[:][::-1])
    chart_weather2.add_rows(df2[:][len(df2)-1: len(df2)])
    status_text2.write(df2.loc[:][::-1])
    time.sleep(1)
