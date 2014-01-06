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
import operator

import sys
sys.path.append('..')
import mp3d.signal
import mp3d.com



distance = 200
distance_small = 10

fig1 = plt.figure()

plt.xlim(0, mp3d.com.T)
plt.ylim(-distance-128, distance+128)
plt.title('test')
plt.xlabel('time (ms)')


l01, = plt.plot([], [], 'r-')
l02, = plt.plot([], [], 'g-')
l03, = plt.plot([], [], 'b-')

l11, = plt.plot([], [], 'r-')
l12, = plt.plot([], [], 'g-')
l13, = plt.plot([], [], 'b-')

l21, = plt.plot([], [], 'r-')
l22, = plt.plot([], [], 'g-')
l23, = plt.plot([], [], 'b-')



def move(y1, diff):
    return [y+diff for y in y1]

def update_line(num):
    mp3d.com.read_all_data()
    idx = 0
    y1, y2, y3 = mp3d.com.get(idx*3+0), mp3d.com.get(idx*3+1), mp3d.com.get(idx*3+2);
    x = arange(0, len(y1)) / mp3d.com.Fsampling_kHz
    l01.set_data(x,move(y1, -distance-distance_small))
    l02.set_data(x,move(y2, -distance))
    l03.set_data(x,move(y3, -distance+distance_small))

    idx = 1
    y1, y2, y3 = mp3d.com.get(idx*3+0), mp3d.com.get(idx*3+1), mp3d.com.get(idx*3+2);
    l11.set_data(x,move(y1, -distance_small))
    l12.set_data(x,move(y2, 0))
    l13.set_data(x,move(y3, +distance_small))

    idx = 2
    y1, y2, y3 = mp3d.com.get(idx*3+0), mp3d.com.get(idx*3+1), mp3d.com.get(idx*3+2);
    l21.set_data(x,move(y1, distance-distance_small))
    l22.set_data(x,move(y2, distance))
    l23.set_data(x,move(y3, distance+distance_small))
    print mp3d.com.time_info
    return


line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=30)
#, blit=True)
#line_ani.save('lines.mp4')

try:
    plt.show()
finally:
    mp3d.com.exit()

