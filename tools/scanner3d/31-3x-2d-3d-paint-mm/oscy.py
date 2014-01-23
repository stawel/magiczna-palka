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
import numpy.linalg

import sys
sys.path.append('..')
import mp3d.signal
import mp3d.com
import mp3d.find_pattern
import mp3d.xyz

mp3d.find_pattern.init_from_file()
szuk_len = len(mp3d.find_pattern.patterns[0])




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

points_nr = mp3d.xyz.points_nr
pos_o = array([0,0,0])
points = array([[0,0,0]]*points_nr)

def setVector(idx, a, b):
    v = np.array([a, b])
    vT= v.T
    vectors3d[idx].set_data(vT[0],vT[1])
    vectors3d[idx].set_3d_properties(vT[2])
def v3d_sqrt(a):
    return a/np.sqrt(numpy.linalg.norm(a))


def update_dot(num):
    global pos_o, points, vectors3d
    t0 = time.time()

    posNx = mp3d.xyz.get_posNx();
    l = len(points)
    if pos_o[0] == 0:
        pos_o = posNx[0]

    for i in range(points_nr):
        points[l-i-1]=posNx[i] - pos_o

    pT=points.T
    dot.set_data(pT[0],pT[1])
    dot2.set_data(pT[0],pT[2])

    dot3d.set_data(pT[0],pT[1])
    dot3d.set_3d_properties(pT[2])

    v0 = v3d_sqrt(mp3d.xyz.best_3d_match_info[0])
    v1 = v3d_sqrt(mp3d.xyz.best_3d_match_info[1])
    
    print 'v1:' ,v1
    setVector(1, points[1], points[1] + v0)
    setVector(0, points[2], points[2] + v1)
    fig3.canvas.draw()


fig3 = plt.figure()
ax = p3.Axes3D(fig3)
vectors3d = [ax.plot(np.array([0,1]),np.array([0,1]),np.array([0,1]),color)[0] for color in ['r','g','b']]
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

cid = fig2.canvas.mpl_connect('key_press_event', onclick)




# Creating the Animation object
dot_ani = animation.FuncAnimation(fig2, update_dot, 25,
                              interval=20, blit=False)


try:
    plt.show()
finally:
    mp3d.xyz.exit()






