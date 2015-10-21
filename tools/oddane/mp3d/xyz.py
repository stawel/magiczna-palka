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
sound_speed = 331.5 + 0.6 * 20
points_nr = 4

find_pattern.init_from_file()

TimeInfo = collections.namedtuple('TimeInfo_xyz', ['all', 'data_first_max', 'get_pos', 'get_gamma', 'refresh_pattern'])
time_info = TimeInfo(0, 0, 0, 0, 0)

def clear_time_info():
    global time_info
    time_info = TimeInfo(0, 0, 0, 0, 0)

def add_time_info(t):
    global time_info
    time_info = TimeInfo(*map(operator.add, time_info, t))

last_measured_position = [0] * com.data_tracks

min_speed_limit = 20000
speed_limit = 20000

def truncate_based_on_speed(idx, szuk_len):
    global speed_limit, min_speed_limit
    y = com.get(idx);
    x = numpy.arange(len(y))
    cut_pos_min = max(0, last_measured_position[idx] - speed_limit)
    cut_pos_max = cut_pos_min + len(find_pattern.patterns[idx]) + 2 * speed_limit
    if cut_pos_min == 0:
        cut_pos_max = len(y)
    speed_limit = max(min_speed_limit, speed_limit * 0.80)
    return x, y, cut_pos_min, x[cut_pos_min:cut_pos_max], y[cut_pos_min:cut_pos_max]


def calculate_pos(idx, truncate_errors = True):
    global time_info
    t0 = time.time()
    pattern_len = len(find_pattern.patterns[idx])
    x, y, cut_x_pos, cut_x, cut_y = truncate_based_on_speed(idx, pattern_len);
    t1 = time.time()
#    errors, corel = find_pattern.get_pos_all(cut_y, idx, truncate_errors=truncate_errors)
#    print 'errors:', errors
    errors, corel = find_pattern.get_pos(cut_y, idx, truncate_errors = truncate_errors)
    t2 = time.time()
    time_info = TimeInfo(t2 - t0, t1 - t0, t2 - t1, 0, 0)
    return  cut_x, cut_y, errors, corel

def refresh(XYE_pos, idxs):
    global last_measured_position
    last_measured_position = []
    for channel in range(len(XYE_pos)):
        x, y, e, c = XYE_pos[channel]
        pos = e[idxs[channel]][1]
        pattern_len = len(find_pattern.patterns[channel])
        find_pattern.refresh_pattern(y[pos:pos + pattern_len], channel)
        last_measured_position.append(x[pos])

    print 'last_measured_position:', last_measured_position

def refresh_nosave(XYE_pos, idxs):
    for channel in range(len(XYE_pos)):
        x, y, e, c = XYE_pos[channel]
        pos = e[idxs[channel]][1]
        pattern_len = len(find_pattern.patterns[channel])
        find_pattern.refresh_pattern(y[pos:pos + pattern_len], channel, save = False)


def to_mm(pos):
    t = pos / com.Fsampling_kHz
    return t * sound_speed


def distance(x, y):
    return numpy.linalg.norm(x - y)

def generate_posNx_by_idx(XYEC_pos, idxs):
    posNx = []
    leng = []
    err = 0.
    for i in range(len(idxs)):
        e = XYEC_pos[i][2]
        x = XYEC_pos[i][0]
        p = (e[0][0], x[e[0][1]])
        if idxs[i] < len(e):
            p= (e[idxs[i]][0], x[e[idxs[i]][1]])
        leng.append(p)

    for i in range(len(idxs) / com.data_channels):
        r1=leng[3 * i + 0][1]
        r2=leng[3 * i + 1][1]
        r3=leng[3 * i + 2][1]
        err = err + leng[3 * i + 0][0] + leng[3 * i + 1][0] + leng[3 * i + 2][0]
        p = get_xyz_pos(to_mm(r1), to_mm(r2), to_mm(r3))
        posNx.append(p)
    return err, posNx

best_3d_match_info = numpy.array([0., 0., 0.]), numpy.array([0., 0., 0.])

def distance_info3d(info1, info2):
    return distance(info1[0], info2[0]) + distance(info1[1], info2[1])

def is_ok_3d_match(info3d):
    return best_3d_match_info[0][0] == 0 or distance_info3d(info3d, best_3d_match_info) < 10000.

def refresh_3d_match(info3d):
    global best_3d_match_info
    best_3d_match_info = info3d

def analize_3d(err,posNx):
    a0, b0 = posNx[0] - posNx[1], posNx[2] - posNx[1]
    c0 = numpy.cross(a0, b0)
    a1, b1 = posNx[1] - posNx[2], posNx[3] - posNx[2]
    c1 = numpy.cross(a1, b1)
    info3d = (c0, c1)
    wsp = distance_info3d(info3d, best_3d_match_info)
    return wsp, info3d

def get_best_3d_match(XYEC_info, max_errors_len):
    output = []
    errors_len = [min(len(i[2]), max_errors_len) for i in XYEC_info]
    print 'errors_len:', errors_len
    for idx in itertools.product(*[range(i) for i in errors_len]):
        err, posNx = generate_posNx_by_idx(XYEC_info, idx)
        wsp, info3d = analize_3d(err, posNx)
        output.append( (wsp, err, idx, posNx, info3d) )

    output.sort(key = operator.itemgetter(0))
    wsp, err, idx, posNx, info3d = output[0]
    print '!!wsp:', wsp,'r:', info3d, 'idx:', idx
    return posNx, idx, info3d

xyec_info = [[]] * com.data_tracks


def get_posNx(permit_refresh = True, force_refresh = False, truncate_errors = True, best_match_error_len = 2):
    global current_distance, xyec_info
    start()
    xyec_info = [calculate_pos(i, truncate_errors = truncate_errors) for i in range(com.data_tracks)]
    posNx, idx, info3d = get_best_3d_match(xyec_info, best_match_error_len)

    if force_refresh or (permit_refresh and is_ok_3d_match(info3d)):
        refresh_3d_match(info3d)
        refresh(xyec_info, idx)
    else:
        refresh_nosave(xyec_info, idx)

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

a = 1000.0
h = a * math.sqrt(3.) / 2.

def get_xyz_pos(dl, up, dr):
    global a, h
    x = (dl**2 - dr**2) / (2. * a)
    y = ((dl**2 - up**2 - (x + a / 2.)**2 + x**2) / h + h) / 2.
    z = (abs(up**2 - x**2 - (y - h)**2))**0.5
    return numpy.array([x, y, z])


def start():
    com.read_all_data()

def exit():
    com.exit()


def get_3x_xyz():
    return 0

