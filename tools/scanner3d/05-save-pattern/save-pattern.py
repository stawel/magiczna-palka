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
import pickle

import sys
sys.path.append('..')
import mp3d.signal
import mp3d.com
import mp3d.find_pattern



distance = 200
distance_small = 20

fig1 = plt.figure()

ax_data = plt.subplot(1,1,1)
plt.xlim(0, mp3d.com.T)
plt.ylim(-distance-128, distance+128)
plt.title('test')
plt.xlabel('time (ms)')




l01, = plt.plot([], [], 'r-')
l02, = plt.plot([], [], 'g-')
l03, = plt.plot([], [], 'b-')

l04, = plt.plot([], [], 'y-')
l05, = plt.plot([], [], 'c-')
l06, = plt.plot([], [], 'm-')


l11, = plt.plot([], [], 'r-')
l12, = plt.plot([], [], 'g-')
l13, = plt.plot([], [], 'b-')

l14, = plt.plot([], [], 'y-')
l15, = plt.plot([], [], 'c-')
l16, = plt.plot([], [], 'm-')

l21, = plt.plot([], [], 'r-')
l22, = plt.plot([], [], 'g-')
l23, = plt.plot([], [], 'b-')

l24, = plt.plot([], [], 'y-')
l25, = plt.plot([], [], 'c-')
l26, = plt.plot([], [], 'm-')


size = 1000



pp = [0 for i in range(size)]

pos = [0,0,0,
       0,0,0,
       0,0,0]

data = [0,0,0, 0,0,0, 0,0,0]

mp3d.find_pattern.init1(pp, 9)


pos2 = mp3d.find_pattern.read_patterns()
if pos2 is not None:
    pos = pos2

def move(y1, diff):
    return [y+diff for y in y1]

def update_line(num):
    global data
    mp3d.com.read_all_data()
    idx = 0

    dist_big   = [-distance, 0, distance]
    dist_small = [-distance_small, 0, distance_small]
    l = [l01,l02,l03,
         l11,l12,l13,
         l21,l22,l23]

    lp =[l04,l05,l06,
         l14,l15,l16,
         l24,l25,l26]

    data = []
    for i in range(9):
        y  = mp3d.com.get(i)
        x  = arange(0, len(y)) / mp3d.com.Fsampling_kHz

        dist = dist_big[i/3]+dist_small[i%3]
        l[i].set_data(x,move(y, dist))
        lp[i].set_data(x[pos[i]:pos[i]+size],move(mp3d.find_pattern.patterns[i], dist))

        data.append(y)
    return


def onclick(event):
    global pos, patterns
    if ax_data == event.inaxes:
        x = int (event.xdata * mp3d.com.Fsampling_kHz)
        if event.ydata < -distance/2:
            idx = 0
            dist = -distance
        elif event.ydata > distance/2:
            idx = 2
            dist = distance
        else:
            idx = 1
            dist = 0
        y = event.ydata-dist

        if y > -distance_small*2 and y < distance_small*2:
            if y < -distance_small/2:
                idy = 0
            elif y > distance_small/2:
                idy = 2
            else:
                idy = 1

            idxy = idx*3+idy
            print idxy
            pos[idxy] = x
            mp3d.find_pattern.patterns[idxy] = data[idxy] [x:x+size];
            mp3d.find_pattern.save_patterns(pos)
    
    print 'button=%d, x=%d, y=%d, xdata=%f, ydata=%f'%(
        event.button, event.x, event.y, event.xdata, event.ydata)


cid = fig1.canvas.mpl_connect('button_press_event', onclick)


line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=30)
#, blit=True)
#line_ani.save('lines.mp4')


plt.show()
mp3d.com.exit()

