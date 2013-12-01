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
import get_data
import get_dolek

fig1 = plt.figure()

plt.xlim(0, 4000)
plt.ylim(0, 256)
plt.title('test')

l1, = plt.plot([], [], 'r-')
l2, = plt.plot([], [], 'g-')
l3, = plt.plot([], [], 'b-')
l4, = plt.plot([], [], 'y-')

lp1, = plt.plot([], [], 'r-')
lp2, = plt.plot([], [], 'g-')
lp3, = plt.plot([], [], 'b-')
lp4, = plt.plot([], [], 'y-')

szuk_org = [128.0, 128.0, 127.0, 128.0, 127.0, 127.0, 128.0, 127.0, 128.0, 127.0, 127.0, 127.0, 127.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 129.0, 128.0, 129.0, 128.0, 128.0, 128.0, 127.0, 127.0, 127.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 126.0, 127.0, 127.0, 128.0, 128.0, 129.0, 130.0, 130.0, 131.0, 131.0, 132.0, 132.0, 132.0, 132.0, 132.0, 133.0, 132.0, 132.0, 132.0, 131.0, 131.0, 130.0, 129.0, 128.0, 127.0, 126.0, 125.0, 124.0, 123.0, 122.0, 122.0, 121.0, 121.0, 120.0, 120.0, 121.0, 121.0, 122.0, 123.0, 124.0, 125.0, 126.0, 127.0, 129.0, 130.0, 132.0, 133.0, 135.0, 136.0, 137.0, 138.0, 138.0, 139.0, 139.0, 139.0, 138.0, 138.0, 137.0, 136.0, 134.0, 132.0, 130.0, 128.0, 126.0, 124.0, 123.0, 120.0, 119.0, 117.0, 116.0, 114.0, 114.0, 113.0, 113.0, 114.0, 114.0, 116.0, 117.0, 119.0, 121.0, 123.0, 125.0, 128.0, 131.0, 133.0, 136.0, 138.0, 140.0, 142.0, 144.0, 145.0, 146.0, 146.0, 146.0, 145.0, 145.0, 143.0, 141.0, 140.0, 137.0, 134.0, 132.0, 129.0, 125.0, 122.0, 120.0, 117.0, 115.0, 113.0, 111.0, 110.0, 109.0, 108.0, 108.0, 109.0, 110.0, 111.0, 113.0, 116.0, 118.0, 121.0, 124.0, 127.0, 130.0, 133.0, 136.0, 139.0, 142.0, 144.0, 146.0, 147.0, 148.0, 148.0, 149.0, 148.0, 148.0, 146.0, 144.0, 142.0, 139.0, 137.0, 134.0, 131.0, 127.0, 124.0, 121.0, 119.0, 116.0, 114.0, 111.0, 110.0, 109.0, 108.0, 108.0, 108.0, 109.0, 110.0, 111.0, 113.0, 115.0, 118.0, 121.0, 123.0, 126.0, 129.0, 132.0, 135.0, 138.0, 140.0, 142.0, 144.0, 145.0, 146.0, 147.0, 147.0, 147.0, 146.0, 145.0, 143.0, 142.0, 140.0, 137.0, 135.0, 132.0, 129.0, 127.0, 124.0, 121.0, 119.0, 117.0, 115.0, 113.0, 112.0, 111.0, 111.0, 111.0, 111.0, 112.0, 113.0, 114.0, 116.0, 118.0, 120.0, 122.0, 124.0, 127.0, 129.0, 132.0, 134.0, 136.0, 138.0, 139.0, 141.0, 141.0, 142.0, 143.0, 143.0, 142.0, 142.0, 141.0, 140.0, 138.0, 137.0, 135.0, 133.0, 131.0, 129.0, 127.0, 125.0, 123.0, 122.0, 120.0, 119.0, 118.0, 117.0, 117.0, 117.0, 117.0, 118.0, 118.0, 119.0, 120.0, 121.0, 123.0, 124.0, 125.0, 127.0, 128.0, 129.0, 131.0, 132.0, 133.0, 133.0, 134.0, 135.0, 135.0, 135.0, 135.0, 135.0, 134.0, 134.0, 133.0, 133.0, 132.0, 131.0, 130.0, 130.0, 129.0, 128.0, 128.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 127.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 128.0, 127.0, 127.0, 126.0, 126.0, 125.0, 125.0, 124.0, 124.0, 124.0, 123.0, 123.0, 123.0, 123.0, 123.0, 124.0, 124.0, 125.0, 126.0, 126.0, 128.0, 128.0, 130.0, 131.0, 133.0, 134.0, 135.0, 136.0, 137.0, 138.0, 138.0, 139.0, 139.0, 139.0, 138.0, 138.0, 136.0, 136.0, 134.0, 133.0, 131.0, 129.0, 127.0, 125.0, 123.0, 121.0, 119.0, 117.0, 115.0, 113.0, 112.0, 112.0, 111.0, 111.0, 111.0, 112.0, 113.0, 114.0, 116.0, 118.0, 121.0, 123.0]
szuk = [[],[],[]]
szuk_avr = sum(szuk_org) / float(len(szuk_org))
for x in szuk_org:
    szuk[0].append(x-szuk_avr)
    szuk[1].append(x-szuk_avr)
    szuk[2].append(x-szuk_avr)

szukc = correlate(szuk[0],szuk[0])[0]
szuk_cor = [szukc,szukc,szukc]
print szuk_cor
#exit()

def refresh_szuk(s,idx):
    global szuk, szuk_cor
    sw_avr = sum(s) / float(len(s))
    sw = []
    for x in s:
        sw.append(x-sw_avr)
    sw_cor = correlate(sw,sw)[0]
    old_cor = correlate(sw,szuk[idx])[0]
    error = get_error(szuk[idx],sw,szuk_cor[idx],old_cor)[1]
    power = sqrt(sw_cor)
    print 'idx: %d  power: %10.3f nowa corelacja: %10.3f  cor old: %10.3f error: %3.10f' % (idx, power, sw_cor , old_cor , error)
    if(error<1. and power > 150.0):
        szuk_cor[idx]=sw_cor
        szuk[idx]=sw


def get_error(xl,yl,xx_cor,xy_cor):
    error_wsp=2.
    error=100
    suma=0
    cor=abs(xy_cor)/xx_cor
    if len(xl) == len(yl):
        for x,y in zip(xl,yl):
            suma+=((y-x*cor)/error_wsp)**2
        error=sqrt(suma/abs(xy_cor))
    return suma/10., error

def get_pos(y,idx):
    avr = sum(y) / float(len(y))
    cy = [x-avr for x in  y]
    data_s=correlate(cy ,szuk[idx])
    data_e = []
    data_c = []
    duze = signal.argrelmax(data_s)[0]
    for j in duze:
        v,e = get_error(szuk[idx],cy[j:j+len(szuk[idx])], szuk_cor[idx], data_s[j])
        data_c.append(v)
        data_e.append(e)
    if len(data_e) > 0:
        pos_i = argmin(data_e)
        pos = duze[pos_i]
    else:
        pos = 0

    return data_s,pos, data_s[pos], data_e 


def cut_arrays(x,y):
    m=min(len(x),len(y))
    return x[:m],y[:m]



def calculate_pos(y,idx):
    t0 = time.time()
    x2,sy,cy,p1,p2,p3,max_r = get_dolek.get_dolek(y,200.)
    cut_pos_max=p3 + int(get_dolek.wave_f)*3
    cut_pos_min=cut_pos_max - len(szuk[idx]) - int(get_dolek.wave_f)*3

    cut_y= y[cut_pos_min:cut_pos_max]
    y2,pos_fk_min, val_cor,y3 = get_pos(cut_y,idx)

    pos_fk_min += cut_pos_min
    pos_fk_max =  pos_fk_min + len(szuk[idx])
    r,gamma,d = get_dolek.get_gamma(y[pos_fk_min:pos_fk_max],200.)
    pos = pos_fk_min -gamma
    refresh_szuk(y[int(math.floor(pos)):int(math.floor(pos))+len(szuk[idx])],idx)
    return pos, time.time()-t0


wyn1 = wyn2 = wyn3 = 0
wyn_ox = wyn_oy = wyn_oz = 0

def update_line(num):
    global wyn1,wyn2,wyn3
    t_data = 0.
    t0 = time.time()
    x,y = get_data.get_data(0);
    t_data+=time.time()-t0;
    l1.set_data(x,y)
    wyn1,t1 = calculate_pos(y,0)
    lp1.set_data([wyn1,wyn1],[0,256])

    t0 = time.time()
    x,y = get_data.get_data(1,-50);
    t_data+=time.time()-t0;
    l2.set_data(x,y)
    wyn2,t2 = calculate_pos(y,1)
    lp2.set_data([wyn2,wyn2],[0,256])

    t0 = time.time()
    x,y = get_data.get_data(2,-100);
    t_data+=time.time()-t0;
    l3.set_data(x,y)
    wyn3,t3 = calculate_pos(y,2)
    lp3.set_data([wyn3,wyn3],[0,256])

    print 'time_data:', t_data, ' time_calc:', t1+t2+t3

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

plt.show()




plt.show()


