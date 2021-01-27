#!/usr/bin/env python3

import serial
import sys
import os
import glob
from datetime import datetime
import matplotlib.pyplot as pp
import numpy as np
import scipy
import time
import struct
import subprocess
import multiprocessing
from config import Configs

CURRENT_VOLTAGE = False
if CURRENT_VOLTAGE:
    numValues = 3
else:
    numValues = 1

READ_RAW = True
DAUGHTER_POWERED = False

port = Configs.portToElasticnode
print(port)
baud = 500e4
ylist = None
datastore = list()
if DAUGHTER_POWERED:
    graphnames = ["wireless", "usb", "fpgaaux", "fpgaint", "daughter", "mcu", "fpgasram", "fpgaio", "charge", "battery",
                  "total"]
else:
    graphnames = ["stateMCU", "wireless", "usb", "fpgaaux", "fpgaint", "daughter", "mcu", "fpgasram", "fpgaio", "total"]


class Measurement:
    # usb, daughter, wireless, mcu, fpgaaux, fpgaint, fpgasram, fpgaio, charge, battery = [None] * (len(graphnames) - 1)
    values = None
    power = None
    calculatedPower = None
    total = None

    def __init__(self, values=None,
                 line=None):  # usb=None, daughter=None, wireless=None, mcu=None, fpgaaux=None, fpgaint=None, fpgasram=None, fpgaio=None, charge=None, battery=None
        if line is not None:
            if len(line) != 4 * numValues * (len(graphnames) - 1):
                print("Not enough bytes in line:", len(line))
                return None
            values = struct.unpack('f' * (numValues * (len(graphnames) - 1)), line)
            # if np.any(np.array(values) > 1):
            #     print ("unrealistically high values...")
            #     print(values)
            #     time.sleep(.5)
            # if np.any(np.array(values) < 0):
            #     print("negative power?")
            #     print(values)
            #     time.sleep(.5)
            # self.wireless, self.usb, self.fpgaaux, self.fpgaint, self.daughter, self.mcu, self.fpgasram, self.fpgaio, self.charge, self.battery = values
            self.values = values
            self.total = self.values[0] + np.sum(self.values[2:-2])

        else:
            self.values = values
            self.total = self.values[0] + np.sum(self.values[2:-2])

    def __repr__(self):
        return (("{:.4f}," * (len(self.values))).format(*self.values))[:-1] + ",{:.4f}".format(self.total)

    def tuple(self):
        return (
        self.usb, self.daughter, self.wireless, self.mcu, self.fpgaaux, self.fpgaint, self.fpgasram, self.fpgaio,
        self.charge, self.battery)

    def array(self):
        if self.values is not None:
            arr = list(self.values)
            arr.append(self.total)
            return arr
            # return self.values
        else:
            return []
        # return [self.usb, self.daughter, self.wireless, self.mcu, self.fpgaaux, self.fpgaint, self.fpgasram, self.fpgaio, self.charge, self.battery]


def datapoint(line):
    # print("usb {} monitor {} wireless {} mcu {} vccaux {} vccint {}".format(usb, monitor, wireless, mcu, vccaux, vccint))
    measurement = Measurement(line)  # usb, monitor, wireless, mcu, fpgaaux, fpgaint)
    datastore.append(measurement)


def read(filename=None):
    if filename is None:
        fn = glob.glob(Configs.pathToProject + "data/*")
        fn.sort()
        fn = fn[-1]
    else:
        fn = Configs.pathToProject + "data/{}.csv".format(filename)

    print("reading", fn)

    with open(fn) as datafile:
        for line in datafile:
            # print ([float(value) for value in line.split(',')[:-1]])
            datastore.append(Measurement(values=[float(value) for value in line.split(',')[:-1]]))


def readFloats(ser):
    # wait for double FF
    target = 0
    while target < 3:

        b, = struct.unpack('B', ser.read(1))
        if int(b) == target + 1:
            # print ("found one", int(b), end=" ")
            target += 1
        elif int(b) == target:
            # print ("same again")
            pass
        else:
            # print("not target", int(b))
            target = 0

    # print("reading")

    data = ser.read((len(graphnames) - 1) * 4 * numValues)

    tail = struct.unpack('BBBBB', ser.read(5))

    tailCorrect = 0
    while tailCorrect < 5:
        if tail[tailCorrect] == 5 - tailCorrect:
            tailCorrect += 1
        else:
            print("wrong tail")
            break

    if tailCorrect == 5:

        dp = Measurement(line=data)
        return dp
    else:
        print("incorrect read...")
        print(data)
        print(tail)
        return None


# def liveplot(dataQueue):
#     ylist = list()
#     trying = True
#     while trying:
#         dp = dataQueue.get()
#         if dp is None:
#             trying = False
#         else:
#             ylist.append(dp.array())
#
#             pp.plot(np.array(ylist))
#             pp.draw()
#             pp.pause(0.1)


def liveread(dataQueue):
    trying = True
    ser = None
    while trying:  # for i in range(100):
        try:
            ser = serial.Serial(port, baud)

            while trying:
                new = readFloats(ser)
                if new is not None:
                    dataQueue.put(new)

        except serial.serialutil.SerialException:
            print("serial not available...")
            time.sleep(0.5)
        except KeyboardInterrupt:
            trying = False
            dataQueue.put(None)
            print("key exc")
        finally:
            print('done')
            if ser is not None:
                ser.close()


# TODO: segmentation fault
def live():
    q = multiprocessing.Queue()

    ylist = list()
    # plotThread = multiprocessing.Process(target=liveplot, args=(q,))
    readThread = multiprocessing.Process(target=liveread, args=(q,))

    # plotThread.start()
    readThread.start()

    try:
        trying = True
        while trying:
            while not q.empty():
                dp = q.get()
                if dp is None:
                    trying = False
                else:
                    ylist.append(dp.array())

            if len(ylist) > 0:
                arr = np.array(ylist)
                max = np.max(arr)
                min = np.min(arr)

                pp.subplot(1, 2, 1)
                pp.cla()
                pp.title("mcu")
                pp.plot(arr[:, 5])  # [:, :10])
                # pp.ylim([min/2, max*2])
                pp.grid()
                pp.subplot(1, 2, 2)
                pp.cla()
                pp.title("power")
                pp.semilogy(arr)  # [:, :10])
                # print(min, max)
                # TODO: non positve number

                pp.ylim([min / 2, int(max * 2)])

                pp.legend(graphnames)
                pp.grid()
                pp.draw()
                pp.pause(0.1)

                np.set_printoptions(suppress=True)

        # plotThread.join()
        readThread.join()
    except KeyboardInterrupt:
        q.put(None)


def debug():
    print("starting debug")
    trying = True
    ser = None

    while trying:  # for i in range(100):
        try:
            ser = serial.Serial(port, baud)

            while trying:
                # new = readFloats(ser)
                print(ser.read(10))
                # if new is not None:
                #     print (new)

            # sys.stdout.write(str(ser.read(1)))
            sys.stdout.flush()
        # except serial.serialutil.portNotOpenError:
        #     print ("serial port not open...")
        #     trying = False
        except serial.serialutil.SerialException:
            print("serial not available...")
            time.sleep(0.5)
        except KeyboardInterrupt:
            trying = False
            print("key exc")
        finally:
            print('done')
            if ser is not None:
                ser.close()


def printout():
    print("starting debug")
    trying = True

    while trying:  # for i in range(100):
        try:
            ser = serial.Serial(port, baud)

            while trying:
                new = readFloats(ser)
                if new is not None:
                    print(new)

            sys.stdout.flush()
        except serial.serialutil.SerialException:
            print("serial not available...")
            time.sleep(0.5)
        except KeyboardInterrupt:
            trying = False
            print("key exc")
        finally:
            print('done')
            try:
                ser.close()
            except:
                print("No connection to close.")


def capture(filename=None):
    print("starting capture")
    ser = serial.Serial(port, baud)
    try:
        os.mkdir(Configs.pathToProject + "data")
    except OSError:
        pass
    print(filename)
    if filename is None or filename == Configs.portToProgrammer:
        fn = Configs.pathToProject + "data/{}.csv".format(datetime.now())
    else:
        fn = Configs.pathToProject + "data/{}.csv".format(filename)
    outputfile = open(fn, 'w')

    # ignore very first value
    first = True

    try:
        while True:  # for i in range(100):

            if READ_RAW:
                new = readFloats(ser)
                # if new is not None:
                #     if not first:
                #         outputfile.write("{}\n".format(new))
                #     else:
                #         first = False

            else:
                line = ser.readline().decode()
                if line.endswith("\r\n"):
                    line = line[:-2]

                try:
                    # usb, monitor, wireless, mcu, vccaux, vccint, newline = line.split(',')
                    new = Measurement(line)

                except ValueError:
                    pass

            if new is not None:
                if not first:
                    outputfile.write("{}\n".format(new))
                else:
                    # outputfile.write("{}\n".format(graphnames))
                    first = False

            # sys.stdout.write(str(ser.read(1)))
            sys.stdout.flush()

    except KeyboardInterrupt:
        print("key exc")
    finally:
        print('done')
        ser.close()
        outputfile.close()


def collect():
    global ylist
    ylist = list()
    # ylist = list()
    for point in datastore:
        ylist.append(point.array())
    ylist = np.array(ylist)


def plot():
    # print(np.array(ylist))
    pp.figure(1)
    pp.cla()
    print(ylist)
    pp.plot(ylist)
    # pp.plot(np.sum(ylist[:, 1:], axis=1))
    pp.legend(graphnames)


ax2 = None


def histograms():
    pp.figure(2)
    bins = np.arange(0, 0.5, 0.01)
    # fig, ax1 = pp.subplots()
    # global ax2
    # ax2 = ax1.twinx()
    print(ylist.shape)
    for i in range(ylist.shape[1]):
        pp.hist(ylist[:, i], bins=bins)
    pp.legend(graphnames)
    # print(np.average(values), np.std(values))


def gauss(x, *p):
    """ Define model function to be used to fit to the data above """
    A, mu, sigma = p
    return A * np.exp(-(x - mu) ** 2 / (2. * sigma ** 2))


def fitModels():
    pp.figure(3)
    print(ylist.shape)
    bins = np.arange(0, 0.5, 1e-4)
    for i in range(ylist.shape[1]):
        mean = np.average(ylist[:, i])
        std = np.std(ylist[:, i])
        print()
        print(graphnames[i])
        if std < 1e-4:
            print("constant", mean, std)
        else:
            print("gauss", mean, std)

        pp.plot(bins, gauss(bins, *(1, mean, std)))
    pp.legend(graphnames)


def show():
    pp.show()


def resetMCU():
    subprocess.call(("avrdude -p atmega32u4 -P " + Configs.portToProgrammer + " -c stk500v2").split(' '))


if __name__ == "__main__":
    deb = False
    cap = True
    reset = False
    drawLive = False
    printDebug = False

    if len(sys.argv) > 2:
        filename = sys.argv[2]
    else:
        filename = None

    if len(sys.argv) > 1:
        deb = "debug" in sys.argv[1]
        cap = "capture" in sys.argv[1]
        reset = "reset" in sys.argv[1]
        drawLive = "live" in sys.argv[1]
        printDebug = "print" in sys.argv[1]
        graph = "graph" in sys.argv[1]
    if drawLive:
        resetMCU()
        live()
    elif printDebug:
        printout()
    elif deb:
        debug()
    elif cap:
        capture(filename)
    elif reset:
        resetMCU()
    elif graph:
        read(filename)
        collect()
        # plot()
        histograms()
        fitModels()
        show()
    else:
        print("No argument given.")
