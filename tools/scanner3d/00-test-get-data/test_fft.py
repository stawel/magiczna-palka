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
import scipy

import sys
sys.path.append('..')
import mp3d.signal
import mp3d.com


fft_size=2048
Fsampling_kHz = mp3d.com.Fsampling_kHz

#Tsampling_ms = 1.0 / Fsampling_kHz

fig1 = plt.figure()

ax_data = plt.subplot(2,1,1)
plt.xlim(7700/Fsampling_kHz, 10000/Fsampling_kHz)
#plt.ylim(-228, 228)
plt.ylim(-200, 200)
plt.xlabel('time (ms)')
plt.title('Data')

l1, = plt.plot([], [], 'r-')
l2, = plt.plot([], [], 'g-')
l3, = plt.plot([], [], 'b-')
l4, = plt.plot([], [], 'y-')
l5, = plt.plot([], [], 'c-')
l6, = plt.plot([], [], 'm-')

plt.subplot(2,1,2)
plt.ylim(-100, 10000)
plt.xlim(0, Fsampling_kHz/2)
plt.xlabel('freq (kHz)')
lp1, = plt.plot([], [], 'r-')
lp2, = plt.plot([], [], 'g-')
lp3, = plt.plot([], [], 'b-')
lp4, = plt.plot([], [], 'y-')
lp5, = plt.plot([], [], 'c-')


fft_p1 = 0
fft_p2 = 0
fft_p3 = 0

def update_line(num):
    idx = 0
    mp3d.com.read_all_data()
    y1, y2, y3 = mp3d.com.get(idx), mp3d.com.get(idx+1), mp3d.com.get(idx+2);
    x = arange(0,len(y1)) / Fsampling_kHz
    y1, y3 = y1-100, y3+100
    l1.set_data(x, y1)
    l2.set_data(x, y2)
    l3.set_data(x, y3)

    fft_x1 = x[fft_p1:fft_p1+fft_size]
    fft_x2 = x[fft_p2:fft_p2+fft_size]
    fft_x3 = x[fft_p3:fft_p3+fft_size]
    fft_y1 = y1[fft_p1:fft_p1+fft_size]
    fft_y2 = y2[fft_p2:fft_p2+fft_size]
    fft_y3 = y3[fft_p3:fft_p3+fft_size]
    l4.set_data(fft_x1, fft_y1)
    l5.set_data(fft_x2, fft_y2)
    l6.set_data(fft_x3, fft_y3)


    fft_x_ax = arange(fft_size/2)*Fsampling_kHz/fft_size


    fft_z1 = abs(scipy.fft(fft_y1))
    fft_z2 = abs(scipy.fft(fft_y2))
    fft_z3 = abs(scipy.fft(fft_y3))
    lp1.set_data(fft_x_ax, fft_z1[:fft_size/2])
    lp2.set_data(fft_x_ax, fft_z2[:fft_size/2])
    lp3.set_data(fft_x_ax, fft_z3[:fft_size/2])
    return


def onclick(event):
    global fft_p1,fft_p2,fft_p3
    if ax_data == event.inaxes:
        x = int (event.xdata * Fsampling_kHz)
        if event.ydata < -50:
            fft_p1 = x
        elif event.ydata > 50:
            fft_p3 = x
        else:
            fft_p2 = x

    print event.inaxes
    print ax_data
    print 'button=%d, x=%d, y=%d, xdata=%f, ydata=%f'%(
        event.button, event.x, event.y, event.xdata, event.ydata)


cid = fig1.canvas.mpl_connect('button_press_event', onclick)

line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=200)
#, blit=True)
#line_ani.save('lines.mp4')


plt.show()
mp3d.com.exit()

