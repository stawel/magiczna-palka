#!/usr/bin/python
from pylab import *
import serial
import time
import math
import collections
import operator
import threading

print "Setting up port"
port_name = '/dev/ttyACM0'
data_channels = 3
data_size = 1024*16*data_channels
Fsampling_kHz = 1600.0
T=data_size/data_channels/Fsampling_kHz


TimeInfo = collections.namedtuple('TimeInfo_com', ['all', 'read', 'write'])
time_info = TimeInfo(0, 0, 0)



def clear_time_info():
    global time_info
    time_info = TimeInfo(0, 0, 0)

def add_time_info(t):
    global time_info
    time_info = TimeInfo(*map(operator.add,time_info, t))


def send_command(port, pin):
    send ='@'
    if pin == 1:
        send = 'A'
    if pin == 2:
        send = 'B'
    port.write(send)
    port.flush()

def read_raw_data(port, diff):
    yData=[]
    raw=port.read(data_size)
    prog_pos=0;

    if len(raw)>0:
        for c in raw:
            t=float(ord(c)) + diff
            yData.append(t)
    return yData

def get_data(port, pin, diff = 0):
    t0 = time.time()
    send_command(port, pin)
    t1 = time.time()
    yData = read_raw_data(port, diff)
    t2 = time.time()

    add_time_info(TimeInfo(0, t2 - t1, t1 - t0))
    return yData

def get_3x_data(port, pin,diff = 0):
    y = get_data(port, pin, diff)
    return y[2::3],y[0::3],y[1::3]


startSig = threading.Event()
endSig   = threading.Event()
exitSig   = threading.Event()

threadLock = threading.RLock()
dataBuf = data =   [[],[],[],
                    [],[],[],
                    [],[],[]]

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
        with serial.Serial(port_name,115200,timeout=2) as port_data:
            while True:
                startSig.wait()
                startSig.clear()
                if exitSig.isSet():
                    return
                clear_time_info()
                t0 = time.time()
                y01, y02, y03 = get_3x_data(port_data,0,-128)
                y11, y12, y13 = get_3x_data(port_data,1,-128)
                y21, y22, y23 = get_3x_data(port_data,2,-128)

                threadLock.acquire()
                dataBuf =  [y01,y02,y03,
                            y11,y12,y13,
                            y21,y22,y23]
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

