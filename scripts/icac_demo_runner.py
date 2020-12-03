#!/usr/bin/env python
import sys
import time
import threading
from matplotlib import pyplot as pp
from icac_powerPlotter import PowerMonitor
from icac_currentMonitoring import CurrentMonitor
from icac_performancePlotter import PerformanceMonitor
from icac_serialReader_master import SerialReaderMaster
from icac_serialReader_monitor import SerialReaderMonitor
from icac_energyPerBatchPlotter import EnergyPerBatchMonitor


#for testing
from mock_serialReaderMaster import SerialMonitorMock

# ./icac_demo_runner.py /dev/ttyUSB0 115200 /dev/ttyACM2 500000

MONITORING_MCU_BAUD = 115200
MONITORING_MCU_PORT = "/dev/ttyACM1"

MASTER_MCU_BAUD = 115200
MASTER_MCU_PORT = "/dev/ttyUSB0"

#TODO: the other thread should also be a list, this is still wrong
def runAllPlots(plotterList, otherThreads):
	print len(plotterList)
	try:
		while True:
			for element in plotterList:
				element.processBufferedDatapoints()
				element.plot()
			# perfMonitor.processBufferedDatapoints()
			# perfMonitor.plot()
			time.sleep(0.05)

	except KeyboardInterrupt:
		print "Keyboard interrupt"
		for element in otherThreads:
			element.stopThread()
			element.join(timeout = 1)
		print "stopped thread"
		print "threads successfully closed"
	finally:
		print "entered finally"
		for element in otherThreads:
			element.stopThread()
			element.join()


# def runCurrentMonitoring():
# 	portMonitor = sys.argv[1]
# 	baudMonitor = sys.argv[2]
	
# 	lockCurrent = threading.Lock()
# 	currentMonitor = CurrentMonitor(lockCurrent)
# 	serialReaderMonitor = SerialReaderMonitor(portMonitor, baudMonitor)
# 	serialReaderMonitor.registerPlotter(currentMonitor)
# 	serialReaderMonitor.daemon = True
# 	serialReaderMonitor.start()

# 	runAllPlots([currentMonitor], [serialReaderMonitor])

# def runTester():
# 	#For the performance
# 	lock = threading.Lock()
# 	perfMonitor = PerformanceMonitor(lock)

# 	monitoringMock = SerialMonitorMock(perfMonitor)
# 	monitoringMock.daemon = True
# 	monitoringMock.start()

# 	runAllPlots(perfMonitor, monitoringMock)

def runDemo():
	portMaster = sys.argv[1]
	baudMaster = sys.argv[2]
	portMonitor = sys.argv[3]
	baudMonitor = sys.argv[4]
	plots = []

	pp.figure()

	serialReaderMaster = SerialReaderMaster(portMaster, baudMaster)
	serialReaderMonitor = SerialReaderMonitor(portMonitor, baudMonitor)
	serialReaderMaster.daemon = True
	serialReaderMonitor.daemon = True

	lockPerf = threading.Lock()
	perfMonitor = PerformanceMonitor(lockPerf)
	serialReaderMaster.registerPlotter(perfMonitor)
	plots.append(perfMonitor)

	lockEnergy = threading.Lock()
	energyMonitor = EnergyPerBatchMonitor(lockEnergy)
	serialReaderMaster.registerPlotter(energyMonitor)
	serialReaderMonitor.registerPlotter(energyMonitor)
	plots.append(energyMonitor)

	# lockCurrent = threading.Lock()
	# currentMonitor = CurrentMonitor(lockCurrent)
	# serialReaderMonitor.registerPlotter(currentMonitor)
	# plots.append(currentMonitor)

	lockPower = threading.Lock()
	powerMonitor = PowerMonitor(lockPower)
	serialReaderMonitor.registerPlotter(powerMonitor)
	plots.append(powerMonitor)


	#here add the current measurement plotter

	serialReaderMaster.start()
	serialReaderMonitor.start()


	runAllPlots(plots, [serialReaderMaster, serialReaderMonitor])
	


# Demo requires the master mcu to:
#	Send the time it took for a batch to complete
#	Send the message that the execution type was switched, by sending what it was switched to
#

if __name__ == '__main__':
	# just runs the test
	# runTester()

	runDemo()

	# runCurrentMonitoring()






#################################################
# Demo protocol:
#
# Master:
# 	General Message Format
#
# 	0xFF,0xFF,<messageType>, data body
#
# 	Message Types:
# 	0x01: batchDuration - body: time as a float?
# 	0x02: executionTypeSwitch - body: typeFpga = 0 typeMcu = 1
#
# Monitor:
#
# 	General Message Format
#
# 	0xFF, <24 bytes for all measurements as floats>, 0xFF








# Alwyn Burger, [12.06.19 17:34]
# performance graph:

# Alwyn Burger, [12.06.19 17:34]
# numberof batches y ticks whole numbers

# Alwyn Burger, [12.06.19 17:34]
# y-axis label add batchsize

# Alwyn Burger, [12.06.19 17:34]
# x axis name

# Alwyn Burger, [12.06.19 17:34]
# power graph:

# Alwyn Burger, [12.06.19 17:35]
# x-axis name

# Alwyn Burger, [12.06.19 17:35]
# FPGA total power

# Alwyn Burger, [12.06.19 17:35]
# Energy graph:

# Alwyn Burger, [12.06.19 17:35]
# y-axis name

# Alwyn Burger, [12.06.19 17:35]
# coours same as performance