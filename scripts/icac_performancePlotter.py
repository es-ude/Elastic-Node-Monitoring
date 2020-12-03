#!/usr/bin/env python

import time
import Queue
import threading
import numpy as np
from matplotlib import pyplot as pp
from icac_serialReader_master import SerialReaderMaster
from icac_serialReader_monitor import SerialReaderMonitor


PERFORMANCE_PLOT_NUMBER = 47

class PerformanceMonitor:

	#Object Variables
	PLOTTING_WINDOW = 100
	
	title = "Elastic Node Performance"
	labelForX = "Time (s)"
	labelForY = "Number of batches Executed"
	maxy = 0
	updating = False

	# figure = None

	#Methods
	def __init__(self, lock, PLOTTING_WINDOW = 100):
		# threading.Thread.__init__(self)
		self.lock = lock
		# self.conditionLock = threading.Condition(threading.Lock())
		
		self.executionType = None # "FPGA" #should receive either MCU or FGPA
		self.batchesExecuted = 0 #reset to zero when type switches

		self.datapoints_x_fpga = [0] #x coords for the plotting
		self.datapoints_y_fpga = [0] #y coords for the plotting

		self.datapoints_x_fpga_larger = [0] #x coords for the plotting
		self.datapoints_y_fpga_larger = [0] #x coords for the plotting
				
		self.datapoints_x_mcu = [0]
		self.datapoints_y_mcu = [0]

		self.datapoints_x_mcu_larger = [0]
		self.datapoints_y_mcu_larger = [0]


		# self.newElementList = []
		self.inputQueue = Queue.Queue()
		self.newDatapointAvailable = False
		
		# pp.figure(PERFORMANCE_PLOT_NUMBER)
		pp.subplot(2,1,1)
		self.drawExtras()
		self.maxy = 0
	
	# #apparently necessary from main thread
	# def updatePlot(self):
	# 	pass

	#threading methods
	# def run(self):
	# 	while not self.finished:
	# 		self.fetchNewDatapoints()
	# 		self.plot()
	# 		time.sleep(0.5)
	
	# def stopThread(self):
	# 	self.finished = True
	

	#Should be called from other threads
	def bufferNewBatchDuration(self, datapoint):

		self.inputQueue.put(datapoint)
		# self.lock.acquire(timeout=1)
		# self.newElementList.append(datapoint)
		self.newDatapointAvailable = True

	def switchExecutionType(self, typeReceived):
		print "perf mon: Switching exec type"
		typeChecked = None
		if typeReceived == "MCU":
			typeChecked = "MCU"

		elif typeReceived == "FPGA":
			typeChecked = "FPGA"

		elif typeReceived == "MCU_LARGER":
			typeChecked = "MCU_LARGER"

		elif typeReceived == "FPGA_LARGER":
			typeChecked = "FPGA_LARGER"
		else:
			print "perf mon: checked type wrong"
			print typeReceived
			return
		
		self.processBufferedDatapoints()

		self.lock.acquire()
		self.updating = True

		print "perf mon: switching type to " + typeReceived
		self.executionType = typeChecked
		print self.executionType

		referenceXVal = self.datapoints_x_fpga[-1]

		self.addZeroPointToAllLines(referenceXVal)

		self.batchesExecuted = 0
		self.updating = False

		self.lock.release()

	def addZeroPointToAllLines(self, referenceXVal):
		self.datapoints_x_mcu.append(referenceXVal)
		self.datapoints_x_fpga.append(referenceXVal)
		self.datapoints_x_mcu_larger.append(referenceXVal)
		self.datapoints_x_fpga_larger.append(referenceXVal)

		self.datapoints_y_mcu.append(0)
		self.datapoints_y_mcu_larger.append(0)
		self.datapoints_y_fpga.append(0)
		self.datapoints_y_fpga_larger.append(0)

	#internals
	def processBufferedDatapoints(self):
		print "per plot: process buffered datapoints"
		if self.newDatapointAvailable:
			listCopy = []
			while not self.inputQueue.empty():
				element = self.inputQueue.get(timeout=1)
				if element is not None:
					listCopy.append(element)
			self.newDatapointAvailable = False

			self.updating = True
			for element in listCopy:
				self.insertDatapoint(element)
			self.updating = False

	#datapoint should be just time in ms or us
	def insertDatapoint(self, datapoint):
		self.batchesExecuted += 1
		if self.executionType == SerialReaderMaster.EXEC_MCU_TYPE_STR:
			# newX  = self.datapoints_x_mcu[-1]
			# newX += datapoint[SerialReaderMaster.KEY_BATCH_DURATION]
			# self.datapoints_x_mcu.append(newX)
			self.datapoints_y_mcu.append(self.batchesExecuted)
			self.datapoints_y_fpga.append(-1)
			self.datapoints_y_mcu_larger.append(-1)
			self.datapoints_y_fpga_larger.append(-1)
		
		elif self.executionType == SerialReaderMaster.EXEC_FPGA_TYPE_STR:
			self.datapoints_y_mcu.append(-1)
			# newX  = self.datapoints_x_fpga[-1]
			# newX += datapoint[SerialReaderMaster.KEY_BATCH_DURATION]
			# self.datapoints_x_fpga.append(newX)
			self.datapoints_y_fpga.append(self.batchesExecuted)
			self.datapoints_y_mcu_larger.append(-1)
			self.datapoints_y_fpga_larger.append(-1)
		
		elif self.executionType == SerialReaderMaster.EXEC_MCU_TYPE_LARGER_STR:
			self.datapoints_y_mcu.append(-1)
			self.datapoints_y_fpga.append(-1)
			# newX  = self.datapoints_x_mcu_larger[-1]
			# newX += datapoint[SerialReaderMaster.KEY_BATCH_DURATION]
			# self.datapoints_x_mcu_larger.append(newX)
			self.datapoints_y_mcu_larger.append(self.batchesExecuted)
			self.datapoints_y_fpga_larger.append(-1)
		
		elif self.executionType == SerialReaderMaster.EXEC_FPGA_TYPE_LARGER_STR:
			self.datapoints_y_mcu.append(-1)
			self.datapoints_y_fpga.append(-1)
			self.datapoints_y_mcu_larger.append(-1)
			# newX  = self.datapoints_x_fpga_larger[-1]
			# newX += datapoint[SerialReaderMaster.KEY_BATCH_DURATION]
			# self.datapoints_x_fpga_larger.append(newX)
			self.datapoints_y_fpga_larger.append(self.batchesExecuted)

		else:
			self.batchesExecuted -= 1
			print "perf mon: executionType was wrong in insertDatapoint"
			print self.executionType
			print datapoint
			return

		if len(self.datapoints_x_mcu) == 0:
			duration = datapoint[SerialReaderMaster.KEY_BATCH_DURATION]
			self.datapoints_x_mcu.append(duration)
			self.datapoints_x_fpga.append(duration)
			self.datapoints_x_mcu_larger.append(duration)
			self.datapoints_x_fpga_larger.append(duration)
		else:
			newX = self.datapoints_x_mcu[-1] + datapoint[SerialReaderMaster.KEY_BATCH_DURATION]
			self.datapoints_x_mcu.append(newX)
			self.datapoints_x_fpga.append(newX)
			self.datapoints_x_mcu_larger.append(newX)
			self.datapoints_x_fpga_larger.append(newX)

		if len(self.datapoints_x_mcu) > self.PLOTTING_WINDOW:
			# someList = [self.datapoints_x_mcu, self.datapoints_x_mcu_larger, self.datapoints_y_mcu, self.datapoints_y_mcu_larger, self.datapoints_x_fpga, self.datapoints_x_fpga_larger, self.datapoints_y_fpga, self.datapoints_y_fpga_larger]
			# for element in someList:
			# 	element = element[-self.PLOTTING_WINDOW:]
			self.datapoints_x_mcu = self.datapoints_x_mcu[-self.PLOTTING_WINDOW:]
			self.datapoints_x_mcu_larger = self.datapoints_x_mcu_larger[-self.PLOTTING_WINDOW:]
			self.datapoints_y_mcu = self.datapoints_y_mcu[-self.PLOTTING_WINDOW:]
			self.datapoints_y_mcu_larger = self.datapoints_y_mcu_larger[-self.PLOTTING_WINDOW:]
			self.datapoints_x_fpga = self.datapoints_x_fpga[-self.PLOTTING_WINDOW:]
			self.datapoints_x_fpga_larger = self.datapoints_x_fpga_larger[-self.PLOTTING_WINDOW:]
			self.datapoints_y_fpga = self.datapoints_y_fpga[-self.PLOTTING_WINDOW:]
			self.datapoints_y_fpga_larger = self.datapoints_y_fpga_larger[-self.PLOTTING_WINDOW:]

	def drawExtras(self):
		pp.ion()
		pp.grid()
		pp.title(self.title)
		pp.xlabel(self.labelForX)
		pp.ylabel(self.labelForY)
	
	def plot(self, timeout=0.05):
		if self.updating: 
			return
		
		if len(self.datapoints_x_mcu) < 1:
			return
		
		# pp.figure(PERFORMANCE_PLOT_NUMBER)
		pp.subplot(2, 1, 1)
		pp.cla()
		self.drawExtras()

		x_list_mcu = np.array(self.datapoints_x_mcu)
		x_list_mcu_larger = np.array(self.datapoints_x_mcu_larger)

		x_list_fpga = np.array(self.datapoints_x_fpga)
		x_list_fpga_larger = np.array(self.datapoints_x_fpga_larger)

		y_list_mcu = np.array(self.datapoints_y_mcu)
		y_list_mcu_larger = np.array(self.datapoints_y_mcu_larger)

		y_list_fpga = np.array(self.datapoints_y_fpga)
		y_list_fpga_larger = np.array(self.datapoints_y_fpga_larger)

		# x_list = np.array(self.datapoints_x) 
		# y_list = np.array(self.datapoints_y)

		# p1, p2 = pp.plot

		# p1, p2 = pp.plot([x_list_mcu, x_list_fpga], [y_list_mcu, y_list_mcu])

		p1, = pp.plot(x_list_mcu, y_list_mcu, color='b', marker='o', linestyle='solid')
		p2, = pp.plot(x_list_fpga, y_list_fpga, color='r', marker='o', linestyle='solid')
		p3, = pp.plot(x_list_mcu_larger, y_list_mcu_larger, color='g', marker='o', linestyle='solid')
		p4, = pp.plot(x_list_fpga_larger, y_list_fpga_larger, color='darkorange', marker='o', linestyle='solid')

		minx = np.min(x_list_mcu)
		maxx = np.max(x_list_mcu)

		some = [y_list_fpga, y_list_fpga_larger, y_list_mcu, y_list_mcu_larger]
		# maxy = np.max(y_list_fpga + y_list_fpga_larger + y_list_mcu + y_list_mcu_larger)
		maxy = np.max(some)

		# maxy = np.min(y_list)

		if minx != maxx:
			pp.xlim([minx, maxx])
			# print np.arange(minx, maxx+1)
			# pp.xticks(np.arange(minx, maxx+1, step=1.0))
		
		if maxy >= 0:
			pp.ylim([0, maxy+1])
		 
		pp.yticks(np.arange(0, maxy+1, step=5.0))
		
		# if self.maxy != 0:
		# 	pp.ylim([0, maxy * 1.1])

		legend = ["MCU:50", "FPGA:50", "MCU:1000", "FPGA:1000"]
		pp.legend([p1, p2, p3, p4], legend, loc='upper right')

		pp.pause(timeout)



# if __name__ == '__main__':
	# pass
