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




def save(name, vl):
    f = open(name, 'w')
    f.write('ply\nformat ascii 1.0\ncomment mp3d stawel\n')
    f.write('element vertex ' + str(len(vl)) + '\n')
    f.write('property float x\nproperty float y\nproperty float z\n')
    f.write('end_header\n')
    for v in vl:
        f.write(str(v[0]) + ' ' + str(v[1]) + ' ' + str(v[1]) + '\n')
