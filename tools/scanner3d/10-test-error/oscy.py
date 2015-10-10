#!/usr/bin/python
# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from pylab import *
import serial
import time
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
import mp3d.xyz
import mp3d.find_pattern
import operator

fig1 = plt.figure()

fft_size=2048*2*2
Fsampling_kHz = mp3d.com.Fsampling_kHz

print_t = False
#Tsampling_ms = 1.0 / Fsampling_kHz

#ax_data = plt.subplot(2,1,1)
if print_t:
    plt.xlim(7700/Fsampling_kHz, 10000/Fsampling_kHz)
else:
    plt.xlim(7700, 10000)

#plt.ylim(-228, 228)
plt.ylim(-200, 200)
if print_t:
    plt.xlabel('czas [ms]')
else:
    plt.xlabel('t')

#plt.title('e(x)')

frame1 = plt.gca()
frame1.axes.get_yaxis().set_visible(False)


l3, = plt.plot([], [], 'r-', label = u'e(t) - błąd względny')
l1, = plt.plot([], [], 'k-', label = u'f(t) - sygnał wejściowy')
l4, = plt.plot([], [], 'y-', label = u'w(x) - wzorzec')
l2, = plt.plot([], [], 'g-', label = u'w(x)*f(t) - korelacja wzajemna')

lp1, = plt.plot([], [], 'r-')
lp2, = plt.plot([], [], 'g-')
lp3, = plt.plot([], [], 'b-')
lp4, = plt.plot([], [], 'y-')

plt.legend()

wyn1 = wyn2 = wyn3 = 0
wyn_ox = wyn_oy = wyn_oz = 0
fref = True
def update_line(num):
    global wyn1,wyn2,wyn3, fref
    mp3d.xyz.start()
    mp3d.xyz.get_posNx(permit_refresh = False, force_refresh = fref, truncate_errors = False, best_match_error_len = 1)
#    fref=False

    idx = 10
    x,y, err1,cor = mp3d.xyz.xyec_info[idx]
    
    if print_t:
        x = x / Fsampling_kHz

    err_idx = err1[0][1]
    wyn1 = x[err_idx]
    l1.set_data(x,y)
#    lp1.set_data([wyn1,wyn1],[-100,0])

    pattern = mp3d.find_pattern.patterns[idx]
    l4.set_data(x[err_idx:err_idx+len(pattern)], pattern - 50);

    l2.set_data(x[:len(cor)], cor/3000. - 120)

    print 'pawel:',len(err1), ' '
    err1.sort(key=operator.itemgetter(1))
    err1T = transpose(err1)

    err1T1 = err1T[1]
    if print_t:
        err1T1 = err1T1/Fsampling_kHz
    l3.set_data(err1T1, err1T[0]*150.000)
#    lp1.set_data([wyn1,wyn1],[-100,100])




line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=30)
#, blit=True)
#line_ani.save('lines.mp4')

# Attaching 3D axis to the figure
fig2 = plt.figure()
ax = p3.Axes3D(fig2)

# Fifty lines of random 3-D lines


# Creating fifty line objects.
# NOTE: Can't pass empty arrays into 3d version of plot()
dot, = ax.plot([0.5],[0.5],[0.5],'bo')

# Setting the axes properties
ax.set_xlim3d([0.0, 100.0])
ax.set_xlabel('X')

ax.set_ylim3d([0.0, 100.0])
ax.set_ylabel('Y')

ax.set_zlim3d([0.0, 100.0])
ax.set_zlabel('Z')

ax.set_title('3D Test')

data = 0


def update_dot(num):
    global wyn_ox,wyn_oy,wyn_oz
    if wyn1*wyn2*wyn3 > 0:
        x,y,z = mp3d.xyz.get_xyz_pos(wyn1 ,wyn2 ,wyn3)
        if wyn_ox == 0:
            wyn_ox=x
            wyn_oy=y
            wyn_oz=z
        dot.set_data([x-wyn_ox],[y-wyn_oy])
        dot.set_3d_properties([z-wyn_oz])

# Creating the Animation object
dot_ani = animation.FuncAnimation(fig2, update_dot, 25,
                              interval=50, blit=False)


try:
    plt.show()
finally:
    mp3d.xyz.exit()


