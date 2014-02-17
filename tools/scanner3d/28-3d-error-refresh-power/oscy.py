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
fref = True
def update_line(num):
    global wyn1,wyn2,wyn3, fref
    mp3d.xyz.start()
    mp3d.xyz.get_posNx(permit_refresh = False, force_refresh = fref, best_match_error_len = 2)
#    fref=False

    x,y, err1,c = mp3d.xyz.xyec_info[0]
    wyn1 = x[err1[0][1]]
    l1.set_data(x,y+100)
    lp1.set_data([wyn1,wyn1],[0,256])

    x,y, err2,c = mp3d.xyz.xyec_info[1]
    wyn2 = x[err2[0][1]]
    l2.set_data(x,y+50)
    lp2.set_data([wyn2,wyn2],[0,256])

    x,y, err3,c = mp3d.xyz.xyec_info[2]
    wyn3 = x[err3[0][1]]
    l3.set_data(x,y)
    lp3.set_data([wyn3,wyn3],[0,256])


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


