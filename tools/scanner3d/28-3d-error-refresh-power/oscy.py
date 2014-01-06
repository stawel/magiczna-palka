#!/usr/bin/python
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

fig1 = plt.figure()

plt.xlim(0, 4000)
plt.ylim(-100, 256)
plt.title('test')

l1, = plt.plot([], [], 'r-')
l2, = plt.plot([], [], 'g-')
l3, = plt.plot([], [], 'b-')
l4, = plt.plot([], [], 'y-')

lp1, = plt.plot([], [], 'r-')
lp2, = plt.plot([], [], 'g-')
lp3, = plt.plot([], [], 'b-')
lp4, = plt.plot([], [], 'y-')


wyn1 = wyn2 = wyn3 = 0
wyn_ox = wyn_oy = wyn_oz = 0

def update_line(num):
    global wyn1,wyn2,wyn3
    mp3d.xyz.start()
    t_data = 0.
    x,y, wyn1 = mp3d.xyz.calculate_pos(0)
    l1.set_data(x,mp3d.xyz.move(y,+100))
    lp1.set_data([wyn1,wyn1],[0,256])

    x,y, wyn2 = mp3d.xyz.calculate_pos(1)
    l2.set_data(x,mp3d.xyz.move(y,+50))
    lp2.set_data([wyn2,wyn2],[0,256])

    x,y, wyn3 = mp3d.xyz.calculate_pos(2)
    l3.set_data(x,y)
    lp3.set_data([wyn3,wyn3],[0,256])

    print 'time_data:', t_data, ' time_calc:'

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


def get_xyz_pos(r1, r2, r3):
    a = 1650./2.
    b = 850.
    y = (r3**2-r1**2)/(4.*a)
    z = ((r1**2-(y-a)**2-r2**2 + y**2)/b+b)/2.
    x = (abs(r1**2 - (y-a)**2 - z**2))**0.5
    print r1,r2,r2, ' -> ' ,x,y,z
    return x,y,z

def update_dot(num):
    global wyn_ox,wyn_oy,wyn_oz
    if wyn1*wyn2*wyn3 > 0:
        x,y,z = get_xyz_pos(wyn1 ,wyn2 ,wyn3)
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


