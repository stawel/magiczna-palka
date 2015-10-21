#!/usr/bin/python
import collections
import time
import math
import numpy
import itertools
import operator
from matplotlib.widgets import Slider, Button, RadioButtons
import matplotlib.animation as animation

import signal
import com
import find_pattern


def add_sl(plt, idx, maxim):
    axcolor = 'lightgoldenrodyellow'
    axfreq = plt.axes([0.25, 0.1 + idx / 15., 0.65, 0.03], axisbg = axcolor)
    sfreq = Slider(axfreq, 's:' + str(idx), 0.1, maxim, valinit = find_pattern.get_power(idx))
    return sfreq


x_axis = numpy.arange(15)
y_axis = [0,1,2, 4,5,6, 8,9,10 ,12,13,14]
y_set  = numpy.array([1,1,1,0,1,1,1,0,1,1,1,0,1,1,1])

def update_sliders(num, x, s1, s2, s3, s4):
    for i in range(12):
        s1[y_axis[i]].set_height(find_pattern.get_sign_power(i))
        s2[y_axis[i]].set_height(find_pattern.get_pattern_power(i))
        s3[y_axis[i]].set_height(find_pattern.get_pos_power(i))
        s4[y_axis[i]].set_height(find_pattern.get_pos_error(i))

def add_sliders(plt):
    global afun
    fig, ax = plt.subplots()
    fig.canvas.set_window_title('power info');
    width = 0.3
    shift = 0.2
    sliders1 = ax.bar(x_axis + 0 * shift, y_set * 100, width * 3, color = 'g' )
    sliders2 = ax.bar(x_axis + 1 * shift, y_set * 100, width, color = 'b')
    sliders3 = ax.bar(x_axis + 2 * shift, y_set * 100, width, color = 'c')
    err4     = ax.bar(x_axis + 3 * shift, y_set * 100, width, color = 'r')

    ax.legend((sliders1[0], sliders2[0], sliders3[0], err4[0]), ('signal power', 'pattern power' , 'position power' , 'position error'))

    afun = animation.FuncAnimation(fig, update_sliders, 25, fargs = (1, sliders1, sliders2, sliders3, err4),
                              interval = 500, blit = False)

