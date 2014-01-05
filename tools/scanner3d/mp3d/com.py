#!/usr/bin/python
from pylab import *
import serial
import time
import math
import collections
import operator

print "Setting up port"
port_data = serial.Serial('/dev/ttyACM0',115200,timeout=2)
data_channels = 3
data_size = 1024*16*data_channels
Fsampling_kHz = 1600.0
T=data_size/data_channels/Fsampling_kHz


TimeInfo = collections.namedtuple('TimeInfo_com', ['all', 'read', 'write'])
time_info = TimeInfo(0, 0, 0)


data = [[],[],[],
        [],[],[],
        [],[],[]]

def clear_time_info():
    global time_info
    time_info = TimeInfo(0, 0, 0)

def add_time_info(t):
    global time_info
    time_info = TimeInfo(*map(operator.add,time_info, t))

def get_data(pin,diff = 0):
    t0 = time.time()
    yData=[]
    send ='@'
    if pin == 1:
        send = 'A'
    if pin == 2:
        send = 'B'


    port_data.write(send)
    port_data.flush()
    t1 = time.time()
    raw=port_data.read(data_size)
    prog_pos=0;

    t2 = time.time()
    if len(raw)>0:
        for c in raw:
            t=float(ord(c)) + diff
            yData.append(t)

    add_time_info(TimeInfo(time.time() - t0, t2 - t1, t1 - t0))
    return yData

def get_3x_data(pin,diff = 0):
    y = get_data(pin, diff)
    return y[2::3],y[0::3],y[1::3]

def get_first_data(pin,diff = 0):
    y1,y2,y3 = get_3x_data(pin, diff)
    return y1


def read_all_data():
    global data
    y01, y02, y03 = get_3x_data(0,-128)
    y11, y12, y13 = get_3x_data(1,-128)
    y21, y22, y23 = get_3x_data(2,-128)
    data = [y01,y02,y03,
            y11,y12,y13,
            y21,y22,y23]


def get(idx):
    return data[idx]
