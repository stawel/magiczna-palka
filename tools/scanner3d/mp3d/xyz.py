#!/usr/bin/python
import collections
import time
import math

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
    y2,pos_fk_min, val_cor,x3,y3 = find_pattern.get_pos(cut_y, idx)
    t2 = time.time()
    pos_fk_min += cut_pos_min
    pos_fk_max =  pos_fk_min + pattern_len
    r,gamma,d = signal.get_gamma(y[pos_fk_min:pos_fk_max])
    t3 = time.time()
    pos = pos_fk_min - gamma
    find_pattern.refresh_pattern(y[int(math.floor(pos)):int(math.floor(pos)) + pattern_len],idx)
    t4 = time.time()
    time_info=TimeInfo(t4 - t0, t1 - t0, t2 - t1, t3 - t2, t4 - t3)
    return x, y, pos


def calculate_pos_mm(idx):
    x , y, pos = calculate_pos(idx)
    t =  pos / com.Fsampling_kHz
    return t*sound_speed

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
    return x,y,z


def start():
    com.read_all_data()

def exit():
    com.exit()


def get_3x_xyz():
    return 0

