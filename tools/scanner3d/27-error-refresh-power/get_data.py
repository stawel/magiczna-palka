#!/usr/bin/python
from pylab import *
import serial
import time
import math

print "Setting up port"
port_data=serial.Serial('/dev/ttyACM0',115200,timeout=2)
data_size=1024*16

def get_data(pin,diff = 0):
    yData=[]
    xData=[]
    send ='@'
    if pin == 1:
        send = 'B'
    if pin == 2:
        send = 'A'


    port_data.write(send)
    port_data.flushInput()
    i=1
    raw=port_data.read(data_size)
    prog_pos=0;
    if len(raw)>0:
        for c in raw:
            t=float(ord(c)) + diff
            yData.append(t)
            xData.append(i)
            i=i+1

    return xData,yData




print get_data(0)


