#!/usr/bin/python
from pylab import *
from numpy import *
import time
import math
import scipy.optimize as optimization
import scipy.signal as signal
import collections
import com
import find_pattern
import operator

wave_f=(3624-3293)/8.
gamma_test_period = 5

TimeInfo = collections.namedtuple('TimeInfo_sig', ['all', 'get_first_max', 'get_pos', 'get_gamma'])
time_info = TimeInfo(0,0,0,0)

def clear_time_info():
    global time_info
    time_info = TimeInfo(0, 0, 0, 0)

def add_time_info(t):
    global time_info
    time_info = TimeInfo(*map(operator.add,time_info, t))


def get_gamma(y):
    time0 = time.time()
    okr = gamma_test_period
    t1=t2=0
    d = []
    w=okr*int(wave_f)
    for i in range(min(w,len(y))):
        t1+=math.sin((i*math.pi*2*okr)/w)*y[i]
        t2+=math.cos((i*math.pi*2*okr)/w)*y[i]
        d.append(math.cos((i*math.pi*2)/w*okr)*10)
    r= (t1**2+t2**2)**0.5
    gamma= math.atan2(t2,t1)
    gamma_px=gamma*w/(math.pi*2*okr)
#    print 'gamma: %+2.10f gamma_px: %+2.10f' % (gamma, gamma_px)

    add_time_info(TimeInfo(0, 0, 0, time.time()-time0))
    return r, gamma_px, d


def get_first_max(y):
    t0 = time.time()
    w = int(math.ceil(wave_f))
    wf = w
    data_s = []
    data_c = []
    data_sin = [[], []]
    x=[]
    for i in range(len(y)/w - 1):
        t1=0
        t2=0
        for j in range(w):
            t = i*w+j
            t_n= int(math.ceil(t))
            s = math.sin((t_n*math.pi*2)/w)
            c = math.cos((t_n*math.pi*2)/w)
            t1+=s*y[t_n]
            t2+=c*y[t_n]
            data_sin[0].append(t_n)
            data_sin[1].append(s*20)
#        r= (t1**2+t2**2)**0.5/w*2
        r= (t1**2+t2**2)**0.5
        gamma= math.atan2(t2,t1)
        data_s.append(r)
        data_c.append(gamma)
        x.append(i*w+w/2)

#    print 'time get_first_max_loop: ', time.time() - t0

    max_r = max(data_s)
    min_r = max_r/4.
#    min_r = 2*max(data_s[:20])
    data_s2 = [max(min_r,a) for a in data_s]
    pos_max2=signal.argrelmax(np.array(data_s2), order=2)[0]
    pos_max = pos_max2[0]*w + w/2

#    print pos_max

    info = x, data_s, data_c, data_sin, max_r, min_r

    add_time_info(TimeInfo(0,time.time()-t0,0,0))
    return pos_max, info


#def get_data_first_max2(idx, szuk_len):
#    x,y = com.get(idx);
#    cut_pos_min=0
#    return x,y,cut_pos_min, x, y


def get_data_first_max(idx, szuk_len):
    t0 = time.time()
    y = com.get(idx);
    x = range(len(y))
    pos_max,info = get_first_max(y)
#    print 'time get_first_max: ', time.time() - t0

    cut_pos_min =     pos_max - szuk_len
    cut_pos_max = cut_pos_min + szuk_len + szuk_len

    cut_y= y[cut_pos_min:cut_pos_max]
    cut_x= x[cut_pos_min:cut_pos_max]

    add_time_info(TimeInfo(time.time()-t0,0,0,0))
    return x,y,cut_pos_min, cut_x, cut_y



