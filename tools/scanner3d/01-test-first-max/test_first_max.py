#!/usr/bin/python
from pylab import *
import serial
from numpy import *
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import mpl_toolkits.mplot3d.axes3d as p3
import math
import scipy.optimize as optimization
import scipy.signal as signal

import sys
sys.path.append('..')
import mp3d.signal
import mp3d.com


fig1 = plt.figure()

plt.xlim(0, 4000)
plt.ylim(-128, 128)
plt.title('test')

l1, = plt.plot([], [], 'r-')
l2, = plt.plot([], [], 'g-')
l3, = plt.plot([], [], 'b-')
l4, = plt.plot([], [], 'y-')
l5, = plt.plot([], [], 'c-')

lp1, = plt.plot([], [], 'r-')
lp2, = plt.plot([], [], 'g-')
lp3, = plt.plot([], [], 'b-')
lp4, = plt.plot([], [], 'y-')
lp5, = plt.plot([], [], 'c-')



def update_line(num):
    mp3d.com.read_all_data()
    mp3d.signal.clear_time_info()
    idx = 2
    y = mp3d.com.get(idx);
    x = range(len(y))
    l1.set_data(x,y)
    p,info  = mp3d.signal.get_first_max(y)
    x2,sy,cy, data_sin , max_r, avr_r = info

    lp2.set_data([p,p],[0,256])

    lp4.set_data([0,8000],[max_r,max_r])
    lp5.set_data([0,8000],[avr_r,avr_r])
    l3.set_data(x2,sy)
    l2.set_data(data_sin)
    print mp3d.signal.time_info
    return


line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=30)
#, blit=True)
#line_ani.save('lines.mp4')

try:
    plt.show()
finally:
    mp3d.com.exit()

