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
points_nr = 4

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
    return x, y, errors, corel

def refresh(XYE_pos, idxs):
    for channel in range(len(XYE_pos)):
        x,y,e,c = XYE_pos[channel]
        pos = e[idxs[channel]][1]
        pattern_len = len(find_pattern.patterns[channel])
        find_pattern.refresh_pattern(y[pos:pos + pattern_len],channel)


def to_mm(pos):
    t =  pos / com.Fsampling_kHz
    return t*sound_speed


def distance(x,y):
    return numpy.linalg.norm(x-y)

def generate_posNx_by_idx(XYEC_pos, idxs):
    posNx = []
    leng = []
    err = 0.
    for i in range(len(idxs)):
        e = XYEC_pos[i][2]
        p = e[0]
        if idxs[i] < len(e):
            p= e[idxs[i]]
        leng.append(p)

    for i in range(len(idxs)/com.data_channels):
        r1=leng[3*i+0][1]
        r2=leng[3*i+1][1]
        r3=leng[3*i+2][1]
        err = err + leng[3*i+0][0] + leng[3*i+1][0] + leng[3*i+2][0]
        p = get_xyz_pos(to_mm(r1),to_mm(r2),to_mm(r3))
        posNx.append(p)
    return err, posNx

def get_short_error_len(errors, errors_len):
    last_err = float('inf');
    leng = 0
    for er in errors:
        if er[0] <= last_err*1.3 and leng < errors_len:
            leng=leng+1
            last_err=er[0]
        else:
            break
    return leng

best_3d_match_info = numpy.array([0.,0.,0.])

def is_ok_3d_match(info3d):
    return best_3d_match_info[0] == 0 or distance(info3d, best_3d_match_info) < 10.

def refresh_3d_match(info3d):
    global best_3d_match_info
    best_3d_match_info = info3d

def get_best_3d_match(errors_short_len, XYEC_pos):
    output = []
    print errors_short_len
    for idx in itertools.product(*[ range(i) for i in errors_short_len]):
        err, posNx = generate_posNx_by_idx(XYEC_pos, idx)
        d01, d02, d12 = distance(posNx[0],posNx[1]), distance(posNx[0],posNx[2]), distance(posNx[1],posNx[2])

        distd = numpy.array([d01, d02, d12])
        r = distance(distd, best_3d_match_info)
        wsp = r
        output.append( (wsp, r,err, idx, distd, posNx) )

    output.sort(key=operator.itemgetter(0))
    d01,d02,d12 = output[0][4]
    posNx = output[0][5]
    r = output[0][1]
    print 'wsp:', output[0][0],'r:', r, ' error:', output[0][2], 'perm:', output[0][3]
    print 'e01: %10.3f  e02: %10.3f  e12: %10.3f' % tuple(numpy.array([d01, d02, d12]) - best_3d_match_info)
    print 'd01: %10.3f  d02: %10.3f  d12: %10.3f' % (d01, d02, d12)
    return posNx, idx, numpy.array([d01,d02,d12])

errors_info = [[]]*com.data_tracks


def get_posNx(permit_refresh = True, force_refresh = False, max_errors_len=2):
    global current_distance, errors_info
    start()
    XYEC_pos = []
    errors_short_len = []
    for i in range(com.data_tracks):
        xyec = calculate_pos(i)
        XYEC_pos.append(xyec)
        errors_short_len.append(get_short_error_len(xyec[2], max_errors_len))
        errors_info[i] = xyec

    posNx, idx, info3d = get_best_3d_match(errors_short_len, XYEC_pos)

#    refresh(XYE_pos,output[0][3])
    if force_refresh or (permit_refresh and is_ok_3d_match(info3d)):
        refresh_3d_match(info3d)
        refresh(XYEC_pos, idx)
    return posNx



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

