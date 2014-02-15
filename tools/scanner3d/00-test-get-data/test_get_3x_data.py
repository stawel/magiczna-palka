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

plt.xlim(7700, 10000)
#plt.ylim(-228, 228)
plt.ylim(-200, 200)
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


idx = 0

def update_line(num):
    global idx
    mp3d.com.read_all_data()
    y1, y2, y3 = mp3d.com.get(idx*3+0), mp3d.com.get(idx*3+1), mp3d.com.get(idx*3+2);
    x1 = range(len(y1))
    x2 = range(len(y2))
    x3 = range(len(y3))
    l1.set_data(x1,y1-100)
    l2.set_data(x2,y2)
    l3.set_data(x3,y3+100)
    return


def onclick(event):
    global idx
    if event.key == 'n':
        idx=(idx+1+4)%4
    if event.key == 'p':
        idx=(idx-1+4)%4
    print idx



cid = fig1.canvas.mpl_connect('key_press_event', onclick)

line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=30)
#, blit=True)
#line_ani.save('lines.mp4')

try:
    plt.show()
finally:
    mp3d.com.exit()

