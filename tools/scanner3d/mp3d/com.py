#!/usr/bin/python
from pylab import *
import serial
import time
import math
import collections
import operator
import threading
import numpy
import scipy.ndimage.filters

print "Setting up port"
port_name = '/dev/ttyACM0'
data_channels = 3
data_tracks = 12
data_size = 1024*16*data_channels
Fsampling_kHz = 1600.0
T=data_size/data_channels/Fsampling_kHz

TimeInfo = collections.namedtuple('TimeInfo_com', ['all', 'read', 'write'])
time_info = TimeInfo(0, 0, 0)
time_info2 = TimeInfo(0, 0, 0)



def clear_time_info():
    global time_info2,time_info
    time_info = time_info2
    time_info2 = TimeInfo(0, 0, 0)

def add_time_info(t):
    global time_info2
    time_info2 = TimeInfo(*map(operator.add,time_info2, t))


def send_command(port, pin):
    send ='@'
    if pin == 1:
        send = 'A'
    if pin == 2:
        send = 'B'
    if pin == 3:
        send = 'C'
    port.write(send)
    port.flush()

def read_raw_data(port, diff):
    raw = port.read(data_size)
    data_array =  numpy.fromstring(raw,dtype='uint8')+diff
    return data_array;

def get_data(port, pin, diff = 0):
    t0 = time.time()
    send_command(port, pin)
    t1 = time.time()
    yData = read_raw_data(port, diff)
    t2 = time.time()

    #print 'read2:', t2-t1
    add_time_info(TimeInfo(0, t2 - t1, t1 - t0))
    return yData


def filter_sign(y):
#    return y
    return scipy.ndimage.filters.gaussian_filter(y.astype(float),5)

def get_3x_data(port, pin,diff = 0):
    y = get_data(port, pin, diff)
    return filter_sign(y[2::3]),filter_sign(y[0::3]),filter_sign(y[1::3])


startSig = threading.Event()
endSig   = threading.Event()
exitSig   = threading.Event()

threadLock = threading.RLock()
dataBuf = data =   [[]] * (data_tracks)

def read_all_data():
    global data
    threadLock.acquire()
    data = dataBuf
    threadLock.release()
    startSig.set()

def get(idx):
    return data[idx]


def doThread():
    global dataBuf
    try:
        with serial.Serial(port_name,1152000,timeout=2) as port_data:
            while True:
                startSig.wait()
                startSig.clear()
                if exitSig.isSet():
                    return
                clear_time_info()
                t0 = time.time()
                localDataBuf = []
                for i in range(data_tracks/data_channels):
                    y1, y2, y3 = get_3x_data(port_data,i,-128)
                    localDataBuf.append(y1)
                    localDataBuf.append(y2)
                    localDataBuf.append(y3)


                #print "doThread timing2:", time.time() - t0
                threadLock.acquire()
                dataBuf =  localDataBuf
                threadLock.release()
                endSig.set();

                add_time_info(TimeInfo(time.time() - t0, 0, 0))

                #print "doThread timing:", time_info
    except IOError:
        print "can't open file:", port_name
        endSig.set();


def exit():
    exitSig.set();
    startSig.set();


#port_data = serial.Serial(port_name,115200,timeout=2)
thread = threading.Thread(target=doThread)
thread.deamon = True
read_all_data()
thread.start()
endSig.wait()
read_all_data()

