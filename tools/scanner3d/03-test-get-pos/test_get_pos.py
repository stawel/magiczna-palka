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
import mp3d.find_pattern
import mp3d.xyz
import operator

fig1 = plt.figure()

plt.xlim(0, 4000)
plt.ylim(-128, 128)
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


#szuk_org = [128.0, 128.0, 127.0, 128.0, 127.0, 127.0, 128.0, 127.0, 128.0, 127.0, 127.0, 127.0, 127.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 128.0, 129.0, 128.0, 128.0, 128.0, 127.0, 127.0, 127.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 127.0, 127.0, 128.0, 128.0, 129.0, 130.0, 130.0, 131.0, 131.0, 132.0, 132.0, 132.0, 132.0, 132.0, 133.0, 132.0, 132.0, 132.0, 131.0, 131.0, 130.0, 129.0, 128.0, 127.0, 126.0, 125.0, 124.0, 123.0, 122.0, 122.0, 121.0, 121.0, 120.0, 120.0, 121.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0, 129.0, 130.0, 132.0, 133.0, 135.0, 136.0, 137.0, 138.0, 138.0, 139.0, 139.0, 139.0, 138.0, 138.0, 137.0, 136.0, 134.0, 132.0, 130.0, 128.0, 126.0, 124.0, 123.0, 120.0, 119.0, 117.0, 116.0, 114.0, 114.0, 113.0, 113.0, 114.0, 114.0, 116.0, 117.0, 119.0, 121.0, 123.0, 125.0, 128.0, 131.0, 133.0, 136.0, 138.0, 140.0, 142.0, 144.0, 145.0, 146.0, 146.0, 146.0, 145.0, 145.0, 143.0, 141.0, 140.0, 137.0, 134.0, 132.0, 129.0, 125.0, 122.0, 120.0, 117.0, 115.0, 113.0, 111.0, 110.0, 109.0, 108.0, 108.0, 109.0, 110.0, 111.0, 113.0, 116.0, 118.0, 121.0, 124.0, 127.0, 130.0, 133.0, 136.0, 139.0, 142.0, 144.0, 146.0, 147.0, 148.0, 148.0, 149.0, 148.0, 148.0, 146.0, 144.0, 142.0, 139.0, 137.0, 134.0, 131.0, 127.0, 124.0, 121.0, 119.0, 116.0, 114.0, 111.0, 110.0, 109.0, 108.0, 108.0, 108.0, 109.0, 110.0, 111.0, 113.0, 115.0, 118.0, 121.0, 123.0, 126.0, 129.0, 132.0, 135.0, 138.0, 140.0, 142.0, 144.0, 145.0, 146.0, 147.0, 147.0, 147.0, 146.0, 145.0, 143.0, 142.0, 140.0, 137.0, 135.0, 132.0, 129.0, 127.0, 124.0, 121.0, 119.0, 117.0, 115.0, 113.0, 112.0, 111.0, 111.0, 111.0, 111.0, 112.0, 113.0, 114.0, 116.0, 118.0, 120.0, 122.0, 124.0, 127.0, 129.0, 132.0, 134.0, 136.0, 138.0, 139.0, 141.0, 141.0, 142.0, 143.0, 143.0, 142.0, 142.0, 141.0, 140.0, 138.0, 137.0, 135.0, 133.0, 131.0, 129.0, 127.0, 125.0, 123.0, 122.0, 120.0, 119.0, 118.0, 117.0, 117.0, 117.0, 117.0, 118.0, 118.0, 119.0, 120.0, 121.0, 123.0, 124.0, 125.0, 127.0, 128.0, 129.0, 131.0, 132.0, 133.0, 133.0, 134.0, 135.0, 135.0, 135.0, 135.0, 135.0, 134.0, 134.0, 133.0, 133.0, 132.0, 131.0, 130.0, 130.0, 129.0, 128.0, 128.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 127.0, 127.0, 126.0, 126.0, 125.0, 125.0, 124.0, 124.0, 124.0, 123.0, 123.0, 123.0, 123.0, 123.0, 124.0, 124.0, 125.0, 126.0, 126.0, 128.0, 128.0, 130.0, 131.0, 133.0, 134.0, 135.0, 136.0, 137.0, 138.0, 138.0, 139.0, 139.0, 139.0, 138.0, 138.0, 136.0, 136.0, 134.0, 133.0, 131.0, 129.0, 127.0, 125.0, 123.0, 121.0, 119.0, 117.0, 115.0, 113.0, 112.0, 112.0, 111.0, 111.0, 111.0, 112.0, 113.0, 114.0, 116.0, 118.0, 121.0, 123.0]

#mp3d.find_pattern.init1(szuk_org,3)
mp3d.find_pattern.init_from_file()

idx = 2
szuk_len = len(mp3d.find_pattern.patterns[idx])

def cut_arrays(x,y):
    m=min(len(x),len(y))
    return x[:m],y[:m]

fref = False

def update_line(num):
    mp3d.xyz.get_posNx(permit_refresh = False, force_refresh = fref, truncate_errors=False, best_match_error_len = 1)

    x,y,errors,cor = mp3d.xyz.xyec_info[idx]

    cut_x=x
    cut_y=cor

    l1.set_data(x,y)
    pos = errors[0][1]
    val_cor = cor[pos]
    errors.sort(key=operator.itemgetter(1))

    l2.set_data(cut_arrays(cut_x, cut_y / 1000.))
    errl = array(errors).T
    l5.set_data(errl[1],errl[0]/10)

#    pos = pos_fk_min

    print pos
    lp2.set_data([pos,pos],[0,256])

    cx= x[pos:(pos+szuk_len)]
    cy= y[pos:(pos+szuk_len)]
    avr_cy = sum(cy)/float(len(cy))
    l3.set_data(cut_arrays(cx, mp3d.find_pattern.patterns[idx] + 195.))
    cor = val_cor/mp3d.find_pattern.patterns_cor[idx]
    if len(cy) == len(mp3d.find_pattern.patterns[idx]):
        lerror = abs((cy-avr_cy)/cor - mp3d.find_pattern.patterns[idx])
        print 'error:',math.sqrt(inner(lerror,lerror))
        l4.set_data(cut_arrays(cx, lerror +avr_cy-100.))

    lp4.set_data(cx,(cy-avr_cy)/cor-100.)
    z = mp3d.find_pattern.patterns[idx]
    l3.set_data(cut_arrays(cx,z-100.))
#    l3.set_data(x2,sy)
    print mp3d.signal.time_info
    print mp3d.find_pattern.time_info



def onclick(event):
    global fref
#    print('you pressed', event.key, event.xdata, event.ydata)
    if event.key == 'x':
        fref = not fref
        print fref



cid = fig1.canvas.mpl_connect('key_press_event', onclick)



line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=30)
#, blit=True)
#line_ani.save('lines.mp4')

try:
    plt.show()
finally:
    mp3d.com.exit()

