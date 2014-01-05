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
import mp3d.signal
import mp3d.com
import mp3d.find_pattern
import mp3d.xyz

mp3d.find_pattern.init_from_file()
szuk_len = len(mp3d.find_pattern.patterns[0])



wyn1 = wyn2 = wyn3 = 0
wyn_ox = wyn_oy = wyn_oz = 0

def update_line(num):
    global wyn1,wyn2,wyn3
    mp3d.xyz.start()
    t_data = 0.
    wyn1 = mp3d.xyz.calculate_pos_mm(0)
    wyn2 = mp3d.xyz.calculate_pos_mm(1)
    wyn3 = mp3d.xyz.calculate_pos_mm(2)
    print wyn1,wyn2,wyn3

fig2 = plt.figure()

limit = 500

plt.xlim(-limit, limit)
plt.ylim(-limit, limit)
plt.title('test')

# Fifty lines of random 3-D lines


# Creating fifty line objects.
# NOTE: Can't pass empty arrays into 3d version of plot()
dot, = plt.plot([0.5],[0.5],'b.')
dot2, = plt.plot([0.5],[0.5],'r.')

# Setting the axes properties
#ax.set_xlim3d([0.0, 100.0])
#ax.set_xlabel('X')

#ax.set_ylim3d([0.0, 100.0])
#ax.set_ylabel('Y')

#ax.set_zlim3d([0.0, 100.0])
#ax.set_zlabel('Z')

#ax.set_title('3D Test')

data = 0


points = [[0],[0],[0]]
x=0
y=0
z=0

dotup = 0
dot_last_up = 0
def update_dot(num):
    global wyn_ox,wyn_oy,wyn_oz,x,y,z,dotup
    t0 = time.time()
    update_line(num)
    dotup+=1
#    print 'time:', time.time()-t0;
    if wyn1*wyn2*wyn3 > 0:
        x,y,z = mp3d.xyz.get_xyz_pos(wyn1 ,wyn2 ,wyn3)
        if wyn_ox == 0:
            wyn_ox=x
            wyn_oy=y
            wyn_oz=z
#        dot.set_data([x-wyn_ox],[y-wyn_oy])
#        dot.set_3d_properties([z-wyn_oz])
        l = len(points[0])
        points[0][l-1]=x-wyn_ox
        points[1][l-1]=y-wyn_oy
        points[2][l-1]=z-wyn_oz
        dot.set_data(points[1],points[2])
        dot2.set_data(points[1],points[0])


fig3 = plt.figure()
ax = p3.Axes3D(fig3)
dot3d, = plt.plot([0.5],[0.5],'g.')

ax.set_xlim3d([-limit, limit])
ax.set_xlabel('X')

ax.set_ylim3d([-limit, limit])
ax.set_ylabel('Y')

ax.set_zlim3d([-limit, limit])
ax.set_zlabel('Z')



def onclick(event):
    global points,wyn_ox,wyn_oy,wyn_oz,x,y,z, dot_last_up, dotup
#    print('you pressed', event.key, event.xdata, event.ydata)
    if event.key == 'c' and dot_last_up != dotup:
        dot_last_up = dotup
        l = len(points[0])
        points[0].append(points[0][l-1])
        points[1].append(points[1][l-1])
        points[2].append(points[2][l-1])

    if event.key == 'x' and len(points[0]) > 1:
        points[0].pop()
        points[1].pop()
        points[2].pop()

    if event.key == '-':
        points = [[0],[0],[0]]

    if event.key == 'p':
        print 'scan:'
        print points

    if event.key == 'o':
        wyn_ox=x
        wyn_oy=y
        wyn_oz=z

    if event.key == 'w':
        dot3d.set_data(points[0],points[1])
        dot3d.set_3d_properties(points[2])
        fig3.canvas.draw()


cid = fig2.canvas.mpl_connect('key_press_event', onclick)




# Creating the Animation object
dot_ani = animation.FuncAnimation(fig2, update_dot, 25,
                              interval=20, blit=False)

plt.show()






