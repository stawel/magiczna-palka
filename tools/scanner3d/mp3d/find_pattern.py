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



pattern = []
pattern_cor = []
def init(sign, dim):
    global pattern, pattern_cor

    avr = sum(sign) / float(len(sign))
    for i in range(dim):
        patt=[]
        for x in sign:
            patt.append(x-avr)
        patt_cor = correlate(patt,patt)[0]
        pattern.append(patt)
        pattern_cor.append(patt_cor)

    print pattern, pattern_cor


def refresh_pattern(sign, idx):
    global pattern, pattern_cor
    sw_avr = sum(sign) / float(len(sign))
    sw = []
    for x in sign:
        sw.append(x-sw_avr)
    sw_cor = correlate(sw,sw)[0]
    old_cor = correlate(sw,pattern[idx])[0]
    error = get_error(pattern[idx],sw,pattern_cor[idx],old_cor)[1]
    power = sqrt(sw_cor)
#    print 'idx: %d  power: %10.3f nowa corelacja: %10.3f  cor old: %10.3f error: %3.10f' % (idx, power, sw_cor , old_cor , error)
    if(error<1. and power > 150.0):
        pattern_cor[idx]=sw_cor
        pattern[idx]=sw


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
    data_s=correlate(cy ,pattern[idx])
    data_e = []
    data_c = []
    duze = signal.argrelmax(data_s)[0]
    for j in duze:
        v,e = get_error(pattern[idx],cy[j:j+len(pattern[idx])], pattern_cor[idx], data_s[j])
        data_c.append(v)
        data_e.append(e)
    if len(data_e) > 0:
        pos_i = argmin(data_e)
        pos = duze[pos_i]
    else:
        pos = 0

    return data_s,pos, data_s[pos], data_e

