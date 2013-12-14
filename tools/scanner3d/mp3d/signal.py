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

wave_f=(3624-3293)/8.
gamma_test_period = 5


TimeInfo = collections.namedtuple('TimeInfo_sig', ['all', 'data_first_max', 'get_pos', 'get_gamma', 'refresh_pattern'])
time_info = TimeInfo(0,0,0,0,0)



def get_gamma(y, diff):
    okr = gamma_test_period
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
#    print 'gamma: %+2.10f gamma_px: %+2.10f' % (gamma, gamma_px)
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

    print 'time get_first_max_loop: ', time.time() - t0

    max_r = max(data_s)
    min_r = max_r/4.
#    min_r = 2*max(data_s[:20])
    data_s2 = [max(min_r,a) for a in data_s]
    pos_max2=signal.argrelmax(np.array(data_s2), order=2)[0]
    pos_max = pos_max2[0]*w + w/2

#    print pos_max

    info = x, data_s, data_c, data_sin, max_r, min_r
    return pos_max, info


def get_data_first_max2(idx, szuk_len, diff = 0):
    x,y = com.get_data(idx, diff);
    cut_pos_min=0
    return x,y,cut_pos_min, x, y


def get_data_first_max(idx, szuk_len, diff = 0):
    x,y = com.get_data(idx, diff);
    t0 = time.time()
    pos_max,info = get_first_max(y)
    print 'time get_first_max: ', time.time() - t0

    cut_pos_min=    pos_max - int(wave_f)*6
    cut_pos_max=cut_pos_min + int(wave_f)*4 + szuk_len

    cut_y= y[cut_pos_min:cut_pos_max]
    cut_x= x[cut_pos_min:cut_pos_max]

    return x,y,cut_pos_min, cut_x, cut_y


def calculate_pos(idx, diff):
    global time_info
    t0 = time.time()
    pattern_len = len(find_pattern.pattern[idx])
    x, y, cut_pos_min, cut_x, cut_y = get_data_first_max(idx, pattern_len, diff);
    t1 = time.time()
    y2,pos_fk_min, val_cor,y3 = find_pattern.get_pos(cut_y, idx)
    t2 = time.time()
    pos_fk_min += cut_pos_min
    pos_fk_max =  pos_fk_min + pattern_len
    r,gamma,d = get_gamma(y[pos_fk_min:pos_fk_max],200.)
    t3 = time.time()
    pos = pos_fk_min - gamma
    find_pattern.refresh_pattern(y[int(math.floor(pos)):int(math.floor(pos)) + pattern_len],idx)
    t4 = time.time()
    time_info=TimeInfo(t4 - t0, t1 - t0, t2 - t1, t3 - t2, t4 - t3)
    return x, y, pos

