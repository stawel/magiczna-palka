#!/usr/bin/python
from pylab import *
import serial
from numpy import *
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import mpl_toolkits.mplot3d.axes3d as p3
import math

print "Setting up port"
port=serial.Serial('/dev/ttyACM0',115200)

start_char = '@';
data_size  = 1024*16;

def get_data(diff = 0):
    yData=[]
    xData=[]
    port.flushInput()

    send = start_char

    port.write(send)
    raw=port.read(data_size)
    i=1
    prog_pos=0;
    if len(raw)>0:
        for c in raw:
            t=float(ord(c)) + diff
            yData.append(t)
            xData.append(i)
            if prog_pos < 1 and i > 125 and yData[-12] - t > 5:
                prog_pos = i
            i=i+1

    return xData,yData

print get_data()


fig1 = plt.figure()

plt.xlim(0, data_size)
plt.ylim(0, 256)
plt.title('test')

l1, = plt.plot([], [], 'r-')

def update_line(num):
    x,y = get_data();
    l1.set_data(x,y)



line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=30)
#, blit=True)
#line_ani.save('lines.mp4')


plt.show()


