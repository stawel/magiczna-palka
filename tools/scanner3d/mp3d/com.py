#!/usr/bin/python
from pylab import *
import serial
import time
import math
import collections

print "Setting up port"
port_data = serial.Serial('/dev/ttyACM0',115200,timeout=2)
data_channels = 3
data_size = 1024*16*data_channels
Fsampling_kHz = 1600.0
T=data_size/data_channels/Fsampling_kHz


TimeInfo = collections.namedtuple('TimeInfo_com', ['all', 'read', 'write'])
time_info = TimeInfo(0, 0, 0)

def get_data(pin,diff = 0):
    global time_info
    t0 = time.time()
    yData=[]
    send ='@'
    if pin == 1:
        send = 'A'
    if pin == 2:
        send = 'B'


    port_data.write(send)
    port_data.flushInput()
    t1 = time.time()
    raw=port_data.read(data_size)
    prog_pos=0;

    t2 = time.time()
    if len(raw)>0:
        for c in raw:
            t=float(ord(c)) + diff
            yData.append(t)

    time_info = TimeInfo(time.time() - t0, t2 - t1, t1 - t0)
    return yData

def get_3x_data(pin,diff = 0):
    y = get_data(pin, diff)
    return y[2::3],y[0::3],y[1::3]

def get_first_data(pin,diff = 0):
    y1,y2,y2 = get_3x_data(pin, diff)
    return y1

