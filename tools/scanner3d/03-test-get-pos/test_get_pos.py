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


szuk_org = [128.0, 128.0, 127.0, 128.0, 127.0, 127.0, 128.0, 127.0, 128.0, 127.0, 127.0, 127.0, 127.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 128.0, 129.0, 128.0, 128.0, 128.0, 127.0, 127.0, 127.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 127.0, 127.0, 128.0, 128.0, 129.0, 130.0, 130.0, 131.0, 131.0, 132.0, 132.0, 132.0, 132.0, 132.0, 133.0, 132.0, 132.0, 132.0, 131.0, 131.0, 130.0, 129.0, 128.0, 127.0, 126.0, 125.0, 124.0, 123.0, 122.0, 122.0, 121.0, 121.0, 120.0, 120.0, 121.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0, 129.0, 130.0, 132.0, 133.0, 135.0, 136.0, 137.0, 138.0, 138.0, 139.0, 139.0, 139.0, 138.0, 138.0, 137.0, 136.0, 134.0, 132.0, 130.0, 128.0, 126.0, 124.0, 123.0, 120.0, 119.0, 117.0, 116.0, 114.0, 114.0, 113.0, 113.0, 114.0, 114.0, 116.0, 117.0, 119.0, 121.0, 123.0, 125.0, 128.0, 131.0, 133.0, 136.0, 138.0, 140.0, 142.0, 144.0, 145.0, 146.0, 146.0, 146.0, 145.0, 145.0, 143.0, 141.0, 140.0, 137.0, 134.0, 132.0, 129.0, 125.0, 122.0, 120.0, 117.0, 115.0, 113.0, 111.0, 110.0, 109.0, 108.0, 108.0, 109.0, 110.0, 111.0, 113.0, 116.0, 118.0, 121.0, 124.0, 127.0, 130.0, 133.0, 136.0, 139.0, 142.0, 144.0, 146.0, 147.0, 148.0, 148.0, 149.0, 148.0, 148.0, 146.0, 144.0, 142.0, 139.0, 137.0, 134.0, 131.0, 127.0, 124.0, 121.0, 119.0, 116.0, 114.0, 111.0, 110.0, 109.0, 108.0, 108.0, 108.0, 109.0, 110.0, 111.0, 113.0, 115.0, 118.0, 121.0, 123.0, 126.0, 129.0, 132.0, 135.0, 138.0, 140.0, 142.0, 144.0, 145.0, 146.0, 147.0, 147.0, 147.0, 146.0, 145.0, 143.0, 142.0, 140.0, 137.0, 135.0, 132.0, 129.0, 127.0, 124.0, 121.0, 119.0, 117.0, 115.0, 113.0, 112.0, 111.0, 111.0, 111.0, 111.0, 112.0, 113.0, 114.0, 116.0, 118.0, 120.0, 122.0, 124.0, 127.0, 129.0, 132.0, 134.0, 136.0, 138.0, 139.0, 141.0, 141.0, 142.0, 143.0, 143.0, 142.0, 142.0, 141.0, 140.0, 138.0, 137.0, 135.0, 133.0, 131.0, 129.0, 127.0, 125.0, 123.0, 122.0, 120.0, 119.0, 118.0, 117.0, 117.0, 117.0, 117.0, 118.0, 118.0, 119.0, 120.0, 121.0, 123.0, 124.0, 125.0, 127.0, 128.0, 129.0, 131.0, 132.0, 133.0, 133.0, 134.0, 135.0, 135.0, 135.0, 135.0, 135.0, 134.0, 134.0, 133.0, 133.0, 132.0, 131.0, 130.0, 130.0, 129.0, 128.0, 128.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 127.0, 127.0, 126.0, 126.0, 125.0, 125.0, 124.0, 124.0, 124.0, 123.0, 123.0, 123.0, 123.0, 123.0, 124.0, 124.0, 125.0, 126.0, 126.0, 128.0, 128.0, 130.0, 131.0, 133.0, 134.0, 135.0, 136.0, 137.0, 138.0, 138.0, 139.0, 139.0, 139.0, 138.0, 138.0, 136.0, 136.0, 134.0, 133.0, 131.0, 129.0, 127.0, 125.0, 123.0, 121.0, 119.0, 117.0, 115.0, 113.0, 112.0, 112.0, 111.0, 111.0, 111.0, 112.0, 113.0, 114.0, 116.0, 118.0, 121.0, 123.0]

mp3d.find_pattern.init1(szuk_org,3)

def cut_arrays(x,y):
    m=min(len(x),len(y))
    return x[:m],y[:m]


def update_line(num):
    idx = 2
    mp3d.com.read_all_data()
    x, y, cut_pos_min, cut_x, cut_y = mp3d.signal.get_data_first_max(idx, len(szuk_org));

    l1.set_data(x,y)
    y2,pos_fk_min, val_cor,y3 = mp3d.find_pattern.get_pos(cut_y,idx)

    l2.set_data(cut_arrays(cut_x,[i/1000+195. for i in y2]))
    l5.set_data(cut_arrays(cut_x,[i+195. for i in y3]))

    pos_fk_min += cut_pos_min
    pos_fk_max =  pos_fk_min + len(szuk_org)
    r,gamma,d = mp3d.signal.get_gamma(y[pos_fk_min:pos_fk_max])
    pos = pos_fk_min-gamma

    pos = pos_fk_min-gamma
    print pos_fk_min, pos
#    print y[int(pos-100):int(pos)+len(szuk[idx])]
    mp3d.find_pattern.refresh_pattern(y[int(math.floor(pos)):int(math.floor(pos))+len(szuk_org)],idx)
    lp2.set_data([pos,pos],[0,256])
#    lp3.set_data([pos_fk_min,pos_fk_min],[0,256])
#    lp1.set_data([cut_pos_min,cut_pos_min],[0,256])
#    lp4.set_data([cut_pos_max,cut_pos_max],[0,256])

    cx= x[pos_fk_min:(pos_fk_min+len(szuk_org))]
    cy= y[pos_fk_min:(pos_fk_min+len(szuk_org))]
    avr_cy = sum(cy)/float(len(cy))
    l3.set_data(cut_arrays(cx,[i+195. for i in mp3d.find_pattern.patterns[idx]]))
    l4.set_data(cut_arrays(cx,[abs(cy[i] -  mp3d.find_pattern.patterns[idx][i]  * val_cor/mp3d.find_pattern.patterns_cor[idx] - avr_cy) +avr_cy  for i in range(min(len(szuk_org),len(cy)))]))
    z = [mp3d.find_pattern.patterns[idx][i]  * val_cor/mp3d.find_pattern.patterns_cor[idx] + avr_cy  for i in range(len(szuk_org)
)]
    l3.set_data(cut_arrays(cx,z))
#    l3.set_data(x2,sy)



line_ani = animation.FuncAnimation(fig1, update_line, None,
    interval=30)
#, blit=True)
#line_ani.save('lines.mp4')

try:
    plt.show()
finally:
    mp3d.com.exit()

