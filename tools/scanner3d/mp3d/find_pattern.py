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
import pickle
import collections
import operator

filename = '/home/stawel/patterns.pkl'


TimeInfo = collections.namedtuple('TimeInfo_fp', ['all', 'refresh_pattern', 'get_pos', 'get_error'])
time_info = TimeInfo(0,0,0,0)

def clear_time_info():
    global time_info
    time_info = TimeInfo(0, 0, 0, 0)

def add_time_info(t):
    global time_info
    time_info = TimeInfo(*map(operator.add,time_info, t))


patterns = []
patterns_cor = []

def read_patterns():
    global patterns
    try:
        with open(filename, 'rb') as f:
            patterns = pickle.load(f)
            extras = pickle.load(f)
            return extras
    except IOError:
        print "can't read file:", filename
    return None

def save_patterns(extras = None):
    global patterns, pos
    try:
        with open(filename, 'wb') as f:
            pickle.dump(patterns, f)
            pickle.dump(extras, f)
    except IOError:
        print "can't write file:", filename



def init(signals):
    global patterns, patterns_cor
    for sign in signals:
        avr = sum(sign) / float(len(sign))
        patt=[]
        for x in sign:
            patt.append(x-avr)
        patt_cor = correlate(patt,patt)[0]
        patterns.append(patt)
        patterns_cor.append(patt_cor)

    #print patterns, patterns_cor


def init1(sign, dim):
    init([sign for i in range(dim)])

def init_from_file():
    global patterns, patterns_cor
    extra = read_patterns()
    p = patterns
    patterns = []
    init(p)

def refresh_pattern(sign, idx):
    global patterns, patterns_cor
    t0 = time.time()
    sw_avr = sum(sign) / float(len(sign))
    sw = []
    for x in sign:
        sw.append(x-sw_avr)
    sw_cor = correlate(sw,sw)[0]
    old_cor = correlate(sw,patterns[idx])[0]
    error = get_error(patterns[idx],sw,patterns_cor[idx],old_cor)[1]
    power = sqrt(sw_cor)
#    print 'idx: %d  power: %10.3f nowa corelacja: %10.3f  cor old: %10.3f error: %3.10f' % (idx, power, sw_cor , old_cor , error)
    if(error<1. and power > 150.0):
        patterns_cor[idx]=sw_cor
        patterns[idx]=sw
    add_time_info(TimeInfo(0, time.time()-t0,0,0))


def get_error(xl,yl,xx_cor,xy_cor):
    t0 = time.time()
    error_wsp=2.
    error=100
    suma=0
    cor=abs(xy_cor)/xx_cor
    if len(xl) == len(yl):
        for x,y in zip(xl,yl):
            suma+=((y-x*cor)/error_wsp)**2
        error=sqrt(suma/abs(xy_cor))

    add_time_info(TimeInfo(0, 0, 0, time.time()-t0))
    return suma/10., error


def get_pos(y,idx):
    t0 = time.time()
    avr = sum(y) / float(len(y))
    cy = [x-avr for x in  y]
    data_s=correlate(cy ,patterns[idx])
    data_e = []
    data_c = []
    duze = signal.argrelmax(data_s)[0]
    for j in duze:
        v,e = get_error(patterns[idx],cy[j:j+len(patterns[idx])], patterns_cor[idx], data_s[j])
        data_c.append(v)
        data_e.append(e)
    if len(data_e) > 0:
        pos_i = argmin(data_e)
        pos = duze[pos_i]
    else:
        pos = 0

    add_time_info(TimeInfo(0, 0, time.time()-t0, 0))
    return data_s,pos, data_s[pos], data_e


