#!/usr/bin/python
import collections
import time
import math
import numpy
import itertools
import operator

import signal
import com
import find_pattern


#sound speed in m/s (temp: 20C)
sound_speed = 331.5+0.6*20

find_pattern.init_from_file()

TimeInfo = collections.namedtuple('TimeInfo_xyz', ['all', 'data_first_max', 'get_pos', 'get_gamma', 'refresh_pattern'])
time_info = TimeInfo(0,0,0,0,0)

def clear_time_info():
    global time_info
    time_info = TimeInfo(0, 0, 0, 0, 0)

def add_time_info(t):
    global time_info
    time_info = TimeInfo(*map(operator.add,time_info, t))


def calculate_pos(idx):
    global time_info
    t0 = time.time()
    pattern_len = len(find_pattern.patterns[idx])
    x, y, cut_pos_min, cut_x, cut_y = signal.get_data_first_max2(idx, pattern_len);
    t1 = time.time()
    errors, corel = find_pattern.get_pos(cut_y, idx)
    t2 = time.time()
    time_info=TimeInfo(t2 - t0, t1 - t0, t2 - t1, 0, 0)
    return x, y, errors

def refresh(XYE_pos, idxs):
    for channel in range(len(XYE_pos)):
        x,y,e = XYE_pos[channel]
        pos = e[idxs[channel]][1]
        pattern_len = len(find_pattern.patterns[channel])
        find_pattern.refresh_pattern(y[pos:pos + pattern_len],channel)


def to_mm(pos):
    t =  pos / com.Fsampling_kHz
    return t*sound_speed

current_distance = numpy.array([0.,0.,0.])

def distance(x,y):
    return numpy.linalg.norm(x-y)

def gen_pos3x(XYE_pos, idxs):
    pos3x = []
    leng = []
    err = 0.
    for i in range(len(idxs)):
        e = XYE_pos[i][2]
        p = e[0]
        if idxs[i] < len(e):
            p= e[idxs[i]]
        leng.append(p)

    for i in range(3):
        r1=leng[3*i+0][1]
        r2=leng[3*i+1][1]
        r3=leng[3*i+2][1]
        err = err + leng[3*i+0][0] + leng[3*i+1][0] + leng[3*i+2][0]
        p = get_xyz_pos(to_mm(r1),to_mm(r2),to_mm(r3))
        pos3x.append(p)
    return err, pos3x

def short_len_errors(errors):
    last_err = float('inf');
    leng = 0
    for er in errors:
        if er[0] <= last_err*1.3 and leng < 4:
            leng=leng+1
            last_err=er[0]
        else:
            break
    return leng

def get_pos3x():
    global current_distance
    start()
    XYE_pos = []
    errors_len = []
    for i in range(9):
        xye = calculate_pos(i)
        XYE_pos.append(xye)
        errors_len.append(short_len_errors(xye[2]))

    output = []
    
    print errors_len
    for e in itertools.product(*[ range(i) for i in errors_len]):
        err, pos3x = gen_pos3x(XYE_pos, e)
        d01, d02, d12 = distance(pos3x[0],pos3x[1]), distance(pos3x[0],pos3x[2]), distance(pos3x[1],pos3x[2])

        distd = numpy.array([d01, d02, d12])
        r = distance(distd, current_distance)
        wsp = r
        output.append( (wsp, r,err,e, distd, pos3x) )

    output.sort(key=operator.itemgetter(0))
    d01,d02,d12 = output[0][4]
    pos3x = output[0][5]
    r = output[0][1]
    print 'wsp:', output[0][0],'r:', r, ' error:', output[0][2], 'perm:', output[0][3]
    print 'e01: %10.3f  e02: %10.3f  e12: %10.3f' % tuple(numpy.array([d01, d02, d12]) - current_distance)
    print 'd01: %10.3f  d02: %10.3f  d12: %10.3f' % (d01, d02, d12)

#    refresh(XYE_pos,output[0][3])
    if current_distance[0] == 0 or r < 10.:
        current_distance = numpy.array([d01,d02,d12])
        refresh(XYE_pos,output[0][3])
    return pos3x



#big triangle dimensions in mm
#     up (0,h,0)
#          / \
#       a / | \
#        /  |h \
#   dl  .---.---. dr
#(-a/2,0,0) ^ (a/2,0,0)
#        (0,0,0)
# dl^2=(x+a/2)^2 + y^2     + z^2
# dr^2=(x-a/2)^2 + y^2     + z^2
# up^2=x^2       + (y-h)^2 + z^2

a = 1520.0
h = a*math.sqrt(3.)/2.

def get_xyz_pos(dl, up, dr):
    global a,h
    x = (dl**2-dr**2)/(2.*a)
    y = ((dl**2-up**2 -(x+a/2.)**2+ x**2)/h+h)/2.
    z = (abs(up**2 - x**2 - (y-h)**2))**0.5
    return numpy.array([x,y,z])


def start():
    com.read_all_data()

def exit():
    com.exit()


def get_3x_xyz():
    return 0

