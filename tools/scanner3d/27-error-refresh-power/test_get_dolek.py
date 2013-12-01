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
import get_data
import get_dolek

print get_data.get_data(0)





fig1 = plt.figure()

plt.xlim(0, 4000)
plt.ylim(0, 256)
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



def update_line(num):
    idx = 2
    x,y = get_data.get_data(idx);
    l1.set_data(x,y)
    x2,sy,cy,p1,p2,p3,max_r = get_dolek.get_dolek(y,200.)

    lp1.set_data([p3,p3],[0,256])
    lp2.set_data([p2,p2],[0,256])
    lp4.set_data([p1,p1],[0,256])
    l3.set_data(x2,sy)
    return


line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=30)
#, blit=True)
#line_ani.save('lines.mp4')


plt.show()


