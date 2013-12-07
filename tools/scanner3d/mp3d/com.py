#!/usr/bin/python
from pylab import *
import serial
import time
import math
import collections

print "Setting up port"
port_data=serial.Serial('/dev/ttyACM0',115200,timeout=2)
data_size=1024*16
TimeInfo = collections.namedtuple('TimeInfo_com', ['all', 'read', 'write'])

time_info = TimeInfo(0, 0, 0)

def get_data(pin,diff = 0):
    global time_info
    t0 = time.time()
    yData=[]
    xData=[]
    send ='@'
    if pin == 1:
        send = 'A'
    if pin == 2:
        send = 'B'


    port_data.write(send)
    port_data.flushInput()
    t1 = time.time()
    i=1
    raw=port_data.read(data_size)
    prog_pos=0;

    t2 = time.time()
    if len(raw)>0:
        for c in raw:
            t=float(ord(c)) + diff
            yData.append(t)
            xData.append(i)
            i=i+1

    time_info = TimeInfo(time.time() - t0, t2 - t1, t1 - t0)
    return xData,yData



