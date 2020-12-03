#!/usr/bin/env python

import time
import threading
import numpy as np
from itertools import repeat
from matplotlib import pyplot as pp
from icac_serialReader_monitor import SerialReaderMonitor

import Queue

PLOTTING_LIMIT = 1000

FULL_LEGEND = ['Total', 'Monitor', 'Wireless', 'MCU', 'FPGA aux', 'FPGA int']

# TOTAL_CURRENT = 0
# MONITOR_CURRENT = 1
# WIRELESS_CURRENT = 2
# MCU_CURRENT = 3
# FPGA_INT_CURRENT = 4
# FPGA_AUX_CURRENT = 5

POWER_PLOT_NUMBER = 89
SHOW_TOTAL = False
class PowerMonitor:

	styles = ['r', 'b', 'g', 'm', 'k', 'c']
	title = "Elastic Node: Power"
	labelForX = "Time (s)"
	labelForY = "Power (mW)"

	latestCurrentDatapoints = []

	power_x_values = []
	power_y_values = [[0],[0],[0],[0],[0],[0]]
	startTime = None
	now = None

	def __init__(self, lock):
		self.lock = lock
		# self.executionType = "FPGA"
		self.updating = False
	
		self.currentMeasurementQueue = Queue.Queue()
		self.newDatapointAvailable = False
		self.now = time.time()
		self.power_x_values.append(0)
		# pp.figure(POWER_PLOT_NUMBER).canvas.set_window_title(self.title)
		pp.subplot(2,2,3)
		self.drawExtras()
	
	def bufferNewCurrentDatapoint(self, datapointCurrent):
		self.currentMeasurementQueue.put(datapointCurrent)
		self.newDatapointAvailable = True
	
	def processBufferedDatapoints(self):
		# print "pow plot: process buffer"
		if not self.newDatapointAvailable:
			return
		self.updating = True
		self.calculateAndStorePowerVals()
		self.newDatapointAvailable = False
		self.updating = False

	def calculateAndStorePowerVals(self):
		while not self.currentMeasurementQueue.empty():
			datapoint = self.currentMeasurementQueue.get()
			self.power_x_values.append(datapoint[SerialReaderMonitor.KEY_TIMESTAMP] - self.now)

			if len(self.power_x_values) > PLOTTING_LIMIT:
				self.power_x_values = self.power_x_values[-PLOTTING_LIMIT:]
			
			voltages = np.array([5, 3.3, 3.3, 3.3, 3.3, 1.2])

			for i in range(len(datapoint[SerialReaderMonitor.KEY_CURRENT_VALUES])):
				powerVal = datapoint[SerialReaderMonitor.KEY_CURRENT_VALUES][i] * voltages[i]
				self.power_y_values[i].append(powerVal)
				if len(self.power_y_values[i]) > PLOTTING_LIMIT:
					self.power_y_values[i] = self.power_y_values[i][-PLOTTING_LIMIT:]

		# 	self.latestCurrentDatapoints.append(self.currentMeasurementQueue.get())
		# if len(self.latestCurrentDatapoints) > PLOTTING_LIMIT:
		# 	self.latestCurrentDatapoints = self.latestCurrentDatapoints[-PLOTTING_LIMIT]
	
	def drawExtras(self):
		# pp.figure(POWER_PLOT_NUMBER)
		pp.subplot(2,2,3)
		pp.ion()
		pp.grid()
		pp.title(self.title)
		pp.xlabel(self.labelForX)
		pp.ylabel(self.labelForY)

	#add options where necessary
	def plot(self, timeout=0.001):
		if self.updating:
			return
		# pp.figure(POWER_PLOT_NUMBER)
		pp.subplot(2, 2, 3)

		pp.cla()
		self.drawExtras()

		x_vals = self.power_x_values
		lines = self.power_y_values
		# fpga_total = lines[-1] + lines[-2]

		fpgaAux = lines[-2]
		fpgaInt = lines[-1]

		fpgaTotal = []

		for i in range(len(fpgaAux)):
			fpgaTotal.append(fpgaAux[i] + fpgaInt[i])

		# print "pow mon: total fpga energy"
		# print fpgaTotal

		lines = lines[:-2]
		lines.append(fpgaTotal)

		colours = self.styles
		labels = FULL_LEGEND[:-2]
		labels.append("FPGA Total")
		
		if not SHOW_TOTAL:
			lines = lines[1:]
			labels = labels[1:]
		
		for line, colour, l in zip(lines, colours, labels):
			pp.plot(np.array(x_vals), line,colour, label='l')
			pp.legend(labels, loc='upper right')

		minx = np.min(x_vals)
		maxx = np.max(x_vals)

		# print "max is "

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
