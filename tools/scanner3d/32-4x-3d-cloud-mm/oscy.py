#!/usr/bin/python
from pylab import *
import serial
import time
from numpy import *
import numpy
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
import mp3d.info
import mp3d.ply


mp3d.find_pattern.init_from_file()


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
dot_end, = plt.plot([0.5],[0.5],'g.')
dot_end2, = plt.plot([0.5],[0.5],'c.')

pos_o = array([0,0,0])
points = [[0,0,0]]

def setVector(idx, a, b):
    v = np.array([a, b])
    vT= v.T
    vectors3d[idx].set_data(vT[0],vT[1])
    vectors3d[idx].set_3d_properties(vT[2])

def v3d_sqrt(a):
    return a/np.sqrt(numpy.linalg.norm(a))

def getC(vl):
    x = array([0.,0.,0.])
    for v in vl:
        x=x+v
    return x/float(len(vl))

pos_cur = numpy.array([0.,0.,0.])
dot_last_up=0
dotup=0
def update_dot(num):
    global pos_o, points, vectors3d, pos_cur, dotup
    t0 = time.time()
    dotup = dotup+1

    posNx = numpy.array(mp3d.xyz.get_posNx())
    l = len(points)
    if pos_o[0] == 0:
        pos_o = getC(posNx)

    for i in range(len(posNx)):
        posNx[i]=posNx[i] - pos_o

    pT=posNx.T
    pos_cur= getC(posNx)
    points[l-1]=pos_cur

    dot.set_data(pT[0],pT[1])
    dot2.set_data(pT[0],pT[2])
    dot3d.set_data(pT[0],pT[1])
    dot3d.set_3d_properties(pT[2])

    pT=numpy.array(points).T
    dot_end.set_data(pT[0],pT[1])
    dot_end2.set_data(pT[0],pT[2])
    dot3d_end.set_data(pT[0],pT[1])
    dot3d_end.set_3d_properties(pT[2])

    v0 = v3d_sqrt(mp3d.xyz.best_3d_match_info[0])
    v1 = v3d_sqrt(mp3d.xyz.best_3d_match_info[1])
    
    print 'v1:' ,v1
    setVector(1, posNx[1], posNx[1] + v0)
    setVector(0, posNx[2], posNx[2] + v1)
    fig3.canvas.draw()
#    mp3d.info.update_sliders()

fig3 = plt.figure()
ax = p3.Axes3D(fig3)
vectors3d = [ax.plot(np.array([0,1]),np.array([0,1]),np.array([0,1]),color)[0] for color in ['r','g','b']]
dot3d, = plt.plot([0.5],[0.5],'g.')
dot3d_end, = plt.plot([0.5],[0.5],'b.')


ax.set_xlim3d([-limit, limit])
ax.set_xlabel('X')

ax.set_ylim3d([-limit, limit])
ax.set_ylabel('Y')

ax.set_zlim3d([-limit, limit])
ax.set_zlabel('Z')


def onclick(event):
    global points,pos_o, pos_cur, dot_last_up, dotup
#    print('you pressed', event.key, event.xdata, event.ydata)
    if event.key == 'c' and dot_last_up != dotup:
        dot_last_up = dotup
        points.append(pos_cur)

    if event.key == 'x' and len(points) > 1:
        points.pop()

    if event.key == '-':
        points = [[0.,0.,0.]]

    if event.key == 'p':
        print 'scan:'
        print points

    if event.key == 'o':
        pos_o = pos_o+pos_cur

    if event.key == 'w':
        mp3d.ply.save('data.ply',points)
        print points
        print 'saved!!!!!!!!!!!!!1'


cid = fig2.canvas.mpl_connect('key_press_event', onclick)


# Creating the Animation object
dot_ani = animation.FuncAnimation(fig2, update_dot, 25,
                              interval=100, blit=False)


mp3d.info.add_sliders(plt)

try:
    plt.show()
finally:
    mp3d.xyz.exit()






