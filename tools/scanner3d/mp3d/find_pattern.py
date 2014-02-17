#!/usr/bin/python
from pylab import *
import serial
from numpy import *
import numpy
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

sign_max_cor = []
pos_pow = []
pos_error = []

def read_patterns():
    global patterns
    try:
        with open(filename, 'rb') as f:
            patterns2 = pickle.load(f)
            extras = pickle.load(f)
            for i in patterns2:
                patterns.append(array(i))
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
    global patterns, patterns_cor, sign_max_cor, pos_pow, pos_error
    for sign in signals:
        avr = sum(sign) / float(len(sign))
        patt= []
        for x in sign:
            patt.append(x-avr)
        patt= array(patt)
        patt_cor = correlate(patt,patt)[0]
        patterns.append(array(patt))
        patterns_cor.append(patt_cor)
        sign_max_cor.append(0.)
        pos_pow.append(0.)
        pos_error.append(0.)

    #print patterns, patterns_cor


def init1(sign, dim):
    init([sign for i in range(dim)])

def init_from_file():
    global patterns, patterns_cor
    extra = read_patterns()
    p = patterns
    patterns = []
    init(p)


def get_pattern_power(idx):
    return sqrt(patterns_cor[idx]/float(len(patterns[idx])))

def get_sign_power(idx):
    return sign_max_cor[idx]/float(len(patterns[idx]))/get_pattern_power(idx)

def get_pos_power(idx):
    return pos_pow[idx]

def get_pos_error(idx):
    return pos_error[idx]




def refresh_pattern(sign, idx, save=True):
    global patterns, patterns_cor, a
    t0 = time.time()
    sw_avr = sum(sign) / float(len(sign))
    sw = sign - sw_avr
#    sw = numpy.sign(array(sw))*50
    sw_cor = correlate(sw, sw)[0]
    old_cor = correlate(sw, patterns[idx])[0]
    error = get_error(patterns[idx], sw, patterns_cor[idx], old_cor)
    power = sqrt(sw_cor/float(len(sign)))
    pos_pow[idx] = power
    pos_error[idx] = error
    if(save and error<1500. and power > 5.0):
        print 'idx: %d  power: %10.3f  sign: %10.3f nowa corelacja: %10.3f  cor old: %10.3f error: %3.10f' % (idx, power, get_sign_power(idx), sw_cor , old_cor , error)
        patterns_cor[idx]=sw_cor
        patterns[idx]=sw
    add_time_info(TimeInfo(0, time.time()-t0,0,0))


def get_error(xl,yl,xx_cor,xy_cor):
    t0 = time.time()
    error=float('inf');
    cor=abs(xy_cor)/xx_cor
    if len(xl) == len(yl):
        yl_avr = sum(yl) / float(len(yl))
        err_array = (((yl-yl_avr)/cor)-xl)
        error = sqrt(inner(err_array,err_array)/ float(len(yl)));

    add_time_info(TimeInfo(0, 0, 0, time.time()-t0))
    return error


def get_pos(y,idx, truncate_errors = True, max_errors_len = 10, truncate_factor = 2):
    t0 = time.time()
    avr = sum(y) / float(len(y))
    cy = y-avr #[x-avr for x in  y]
    corel = correlate(cy ,patterns[idx])
    errors = [(float('inf'),0)]
    duze = signal.argrelmax(corel)[0]
    for px in duze:
        e = get_error(patterns[idx],cy[px:px+len(patterns[idx])], patterns_cor[idx], corel[px])
        errors.append( (e,px) )

    errors.sort(key=operator.itemgetter(0))

    if truncate_errors:
        t_errors=[]
        first_err = errors[0][0];
        errors = errors[:max_errors_len]
        for er in errors:
            if er[0] <= first_err*truncate_factor:
                t_errors.append(er)
            else:
                break
        errors=t_errors

    sign_max_cor[idx] = amax(corel)

    add_time_info(TimeInfo(0, 0, time.time()-t0, 0))
    return errors, corel


