#!/usr/bin/python
from pylab import *
from numpy import *
import time
import math
import scipy.optimize as optimization
import scipy.signal as signal

wave_f=(3624-3293)/8.
minimum_signal = 220
gamma_okr = 5


def get_gamma(y, diff):
    okr = gamma_okr
    t1=t2=0
    d = []
    w=okr*int(wave_f)
    for i in range(min(w,len(y))):
        t1+=math.sin((i*math.pi*2*okr)/w)*y[i]
        t2+=math.cos((i*math.pi*2*okr)/w)*y[i]
        d.append(math.cos((i*math.pi*2)/w*okr)*10+diff)
    r= (t1**2+t2**2)**0.5
    gamma= math.atan2(t2,t1)
    gamma_px=gamma*w/(math.pi*2*okr)
    print 'gamma: %+2.10f gamma_px: %+2.10f' % (gamma, gamma_px)
    return r, gamma_px, d


def get_dolek(y,diff):
    w = int(math.ceil(wave_f))
    wf = wave_f
    data_s=[]
    data_c=[]
    x=[]
    pos_start = pos_max = pos_end = pos_min = last_r = max_r = min_r = 0
    for i in range(len(y)/w - 1):
        t1=0
        t2=0
        for j in range(w):
            t1+=math.sin(((i*w+j)*math.pi*2)/wf)*y[i*w+j]
            t2+=math.cos(((i*w+j)*math.pi*2)/wf)*y[i*w+j]
        r= (t1**2+t2**2)**0.5
        gamma= math.atan2(t2,t1)
        data_s.append(r+diff)
        data_c.append(gamma+diff)
        x.append(i*w)



        if pos_start < 1 and r > minimum_signal:
            pos_start = i*w

        if pos_start > 1 and pos_end < 1 and max_r < r and max_r == min_r:
            pos_max = i*w- w*gamma/(math.pi*2)
            min_r = r
            max_r = r

        if pos_start > 1 and pos_end < 1 and min_r > r:
            min_r = r
            pos_min = (i+1)*w

        if pos_start > 1 and pos_end < 1 and r > min_r:
            pos_end = pos_min

        last_r = r

    return x,data_s,data_c,pos_start, pos_max, pos_end,max_r

