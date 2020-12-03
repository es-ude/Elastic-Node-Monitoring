#!/usr/bin/env python

import time
import threading
import numpy as np
from itertools import repeat
from matplotlib import pyplot as pp
from icac_serialReader_monitor import SerialReaderMonitor

import Queue

PLOTTING_LIMIT = 500

FULL_LEGEND = ['Total', 'Monitor', 'Wireless', 'MCU', 'FPGA int', 'FPGA aux']

# TOTAL_CURRENT = 0
# MONITOR_CURRENT = 1
# WIRELESS_CURRENT = 2
# MCU_CURRENT = 3
# FPGA_INT_CURRENT = 4
# FPGA_AUX_CURRENT = 5

CURRENT_PLOT_NUMBER = 42
class CurrentMonitor:

	styles = ['r', 'b', 'g', 'm', 'k', 'c']
	title = "Elastic Node: Current"
	labelForX = "time (ms)"
	labelForY = "Current (mA)"

	latestCurrentDatapoints = []

	current_x_values = []
	current_y_values = [[0],[0],[0],[0],[0],[0]]

	def __init__(self, lock):
		self.lock = lock
		# self.executionType = "FPGA"
		self.updating = False
	
		self.currentMeasurementQueue = Queue.Queue()
		self.newDatapointAvailable = False
		now = time.time()
		self.current_x_values.append(now)
		pp.figure(CURRENT_PLOT_NUMBER).canvas.set_window_title(self.title)
		self.drawExtras()
	
	def bufferNewCurrentDatapoint(self, datapointCurrent):
		self.currentMeasurementQueue.put(datapointCurrent)
		self.newDatapointAvailable = True
	
	def processBufferedDatapoints(self):
		print "c plot: process buffer"
		if not self.newDatapointAvailable:
			return
		self.updating = True
		self.storeBufferedCurrentDatapoints()
		self.newDatapointAvailable = False
		self.updating = False

	def storeBufferedCurrentDatapoints(self):
		while not self.currentMeasurementQueue.empty():
			datapoint = self.currentMeasurementQueue.get()
			self.current_x_values.append(datapoint[SerialReaderMonitor.KEY_TIMESTAMP])
			if len(self.current_x_values) > PLOTTING_LIMIT:
				# print "cur plot: reached plotting limit, shortening"
				# print len(self.current_x_values)
				# print type(self.current_x_values)
				self.current_x_values = self.current_x_values[-PLOTTING_LIMIT:]
			for i in range(len(datapoint[SerialReaderMonitor.KEY_CURRENT_VALUES])):
				self.current_y_values[i].append(datapoint[SerialReaderMonitor.KEY_CURRENT_VALUES][i])
				if len(self.current_y_values[i]) > PLOTTING_LIMIT:
					self.current_y_values[i] = self.current_y_values[i][-PLOTTING_LIMIT:]

		# 	self.latestCurrentDatapoints.append(self.currentMeasurementQueue.get())
		# if len(self.latestCurrentDatapoints) > PLOTTING_LIMIT:
		# 	self.latestCurrentDatapoints = self.latestCurrentDatapoints[-PLOTTING_LIMIT]
	
	def drawExtras(self):
		pp.figure(CURRENT_PLOT_NUMBER)
		pp.ion()
		pp.grid()
		pp.title(self.title)
		pp.xlabel(self.labelForX)
		pp.ylabel(self.labelForY)

	#add options where necessary
	def plot(self, timeout=0.05):
		if self.updating: 
			return
		pp.figure(CURRENT_PLOT_NUMBER)
		pp.cla()
		self.drawExtras()

		x_vals = self.current_x_values
		lines = self.current_y_values
		colours = self.styles
		labels = FULL_LEGEND
		
		for line, colour, l in zip(lines, colours, labels):
			pp.plot(x_vals, line,colour, label='l')
			pp.legend(labels)

		minx = np.min(x_vals)
		maxx = np.max(x_vals)

		maxy = 0

		for line in lines:
			lineMax = np.max(line)
			if lineMax > maxy:
				maxy = lineMax
				
		if minx != maxx:
			pp.xlim([minx, maxx])
		if maxy != 0:
			pp.ylim([0, maxy * 1.1])

		pp.pause(timeout)

# if __name__ == '__main__':
# 	lock=threading.Lock()
# 	currentMonitor = CurrentMonitor(lock)
# 	now = time.time()
# 	currentMonitor.current_x_values = np.arange(1,10) + now
# 	for i in range(0, 6):
# 		currentMonitor.current_y_values[i] = currentMonitor.current_x_values * i
	
# 	currentMonitor.plot()