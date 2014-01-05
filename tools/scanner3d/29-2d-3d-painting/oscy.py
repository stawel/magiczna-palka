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

#szuk_org = [128.0, 128.0, 127.0, 128.0, 127.0, 127.0, 128.0, 127.0, 128.0, 127.0, 127.0, 127.0, 127.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 128.0, 129.0, 128.0, 128.0, 128.0, 127.0, 127.0, 127.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 127.0, 127.0, 128.0, 128.0, 129.0, 130.0, 130.0, 131.0, 131.0, 132.0, 132.0, 132.0, 132.0, 132.0, 133.0, 132.0, 132.0, 132.0, 131.0, 131.0, 130.0, 129.0, 128.0, 127.0, 126.0, 125.0, 124.0, 123.0, 122.0, 122.0, 121.0, 121.0, 120.0, 120.0, 121.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0, 129.0, 130.0, 132.0, 133.0, 135.0, 136.0, 137.0, 138.0, 138.0, 139.0, 139.0, 139.0, 138.0, 138.0, 137.0, 136.0, 134.0, 132.0, 130.0, 128.0, 126.0, 124.0, 123.0, 120.0, 119.0, 117.0, 116.0, 114.0, 114.0, 113.0, 113.0, 114.0, 114.0, 116.0, 117.0, 119.0, 121.0, 123.0, 125.0, 128.0, 131.0, 133.0, 136.0, 138.0, 140.0, 142.0, 144.0, 145.0, 146.0, 146.0, 146.0, 145.0, 145.0, 143.0, 141.0, 140.0, 137.0, 134.0, 132.0, 129.0, 125.0, 122.0, 120.0, 117.0, 115.0, 113.0, 111.0, 110.0, 109.0, 108.0, 108.0, 109.0, 110.0, 111.0, 113.0, 116.0, 118.0, 121.0, 124.0, 127.0, 130.0, 133.0, 136.0, 139.0, 142.0, 144.0, 146.0, 147.0, 148.0, 148.0, 149.0, 148.0, 148.0, 146.0, 144.0, 142.0, 139.0, 137.0, 134.0, 131.0, 127.0, 124.0, 121.0, 119.0, 116.0, 114.0, 111.0, 110.0, 109.0, 108.0, 108.0, 108.0, 109.0, 110.0, 111.0, 113.0, 115.0, 118.0, 121.0, 123.0, 126.0, 129.0, 132.0, 135.0, 138.0, 140.0, 142.0, 144.0, 145.0, 146.0, 147.0, 147.0, 147.0, 146.0, 145.0, 143.0, 142.0, 140.0, 137.0, 135.0, 132.0, 129.0, 127.0, 124.0, 121.0, 119.0, 117.0, 115.0, 113.0, 112.0, 111.0, 111.0, 111.0, 111.0, 112.0, 113.0, 114.0, 116.0, 118.0, 120.0, 122.0, 124.0, 127.0, 129.0, 132.0, 134.0, 136.0, 138.0, 139.0, 141.0, 141.0, 142.0, 143.0, 143.0, 142.0, 142.0, 141.0, 140.0, 138.0, 137.0, 135.0, 133.0, 131.0, 129.0, 127.0, 125.0, 123.0, 122.0, 120.0, 119.0, 118.0, 117.0, 117.0, 117.0, 117.0, 118.0, 118.0, 119.0, 120.0, 121.0, 123.0, 124.0, 125.0, 127.0, 128.0, 129.0, 131.0, 132.0, 133.0, 133.0, 134.0, 135.0, 135.0, 135.0, 135.0, 135.0, 134.0, 134.0, 133.0, 133.0, 132.0, 131.0, 130.0, 130.0, 129.0, 128.0, 128.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 127.0, 127.0, 126.0, 126.0, 125.0, 125.0, 124.0, 124.0, 124.0, 123.0, 123.0, 123.0, 123.0, 123.0, 124.0, 124.0, 125.0, 126.0, 126.0, 128.0, 128.0, 130.0, 131.0, 133.0, 134.0, 135.0, 136.0, 137.0, 138.0, 138.0, 139.0, 139.0, 139.0, 138.0, 138.0, 136.0, 136.0, 134.0, 133.0, 131.0, 129.0, 127.0, 125.0, 123.0, 121.0, 119.0, 117.0, 115.0, 113.0, 112.0, 112.0, 111.0, 111.0, 111.0, 112.0, 113.0, 114.0, 116.0, 118.0, 121.0, 123.0]
#mp3d.find_pattern.init(szuk_org, 3)

mp3d.find_pattern.init_from_file()
szuk_len = len(mp3d.find_pattern.patterns[0])


def cut_arrays(x,y):
    m=min(len(x),len(y))
    return x[:m],y[:m]


wyn1 = wyn2 = wyn3 = 0
wyn_ox = wyn_oy = wyn_oz = 0

def update_line(num):
    global wyn1,wyn2,wyn3
    mp3d.xyz.start()
    t_data = 0.
    x,y, wyn1 = mp3d.xyz.calculate_pos(0)
    x,y, wyn2 = mp3d.xyz.calculate_pos(1)
    x,y, wyn3 = mp3d.xyz.calculate_pos(2)
#    lp3.set_data([wyn3,wyn3],[0,256])

#    print 'time_data:', t_data, ' time_calc:', t1+t2+t3

#line_ani = animation.FuncAnimation(fig1, update_line, None,
#    interval=30)

#, blit=True)
#line_ani.save('lines.mp4')

# Attaching 3D axis to the figure

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


def get_xyz_pos(r1, r2, r3):
    a = 1650./2.
    b = 850.
    y = (r3**2-r1**2)/(4.*a)
    z = ((r1**2-(y-a)**2-r2**2 + y**2)/b+b)/2.
    x = (abs(r1**2 - (y-a)**2 - z**2))**0.5
#    print r1,r2,r2, ' -> ' ,x,y,z
    return x,y,z


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
        x,y,z = get_xyz_pos(wyn1 ,wyn2 ,wyn3)
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






