#!/usr/bin/env python

import time
import array
import threading
import numpy as np

# from icac_performancePlotter import PerformanceMonitor
from lib_serialCharReader import SerialCharReader

class SerialReaderMonitor(threading.Thread):
	KEY_TIMESTAMP = "timestamp"
	KEY_CURRENT_VALUES ="currentValues"
	serialDevice = None

	plotterList = []

	def __init__(self, port, baud):
		threading.Thread.__init__(self)
		# self.perfMonitor = perfMonitor
		self.finished = False
		self.port = port
		self.baud = baud

	def registerPlotter(self, plotterObject):
		if hasattr(plotterObject, "bufferNewCurrentDatapoint"):
			self.plotterList.append(plotterObject)
	
	def run(self):
		self.initialise()
		while not self.finished:
			self.processMessage()

	def initialise(self):
		self.serialDevice = SerialCharReader(self.port, self.baud, timeout = None)
		self.serialDevice.openSerial()

	def stopThread(self):
		self.finished = True

	#add new messages here
	def processMessage(self):
		# print "ser mon: entered process message"
		currentValues = self.getRawCurrentDatapoint()
		if currentValues is None:
			print "ser mon: the raw dp is None"
			return
		now = time.time()
		#check for unreasonable values
		for element in currentValues:
			if element > 1000.0:
				print "ser mon: element > 1000.0. was unreasonable"
				return

		currentList = []
		for element in currentValues:
			currentList.append(element)

		datapoint = {SerialReaderMonitor.KEY_TIMESTAMP: now, SerialReaderMonitor.KEY_CURRENT_VALUES: currentList}
		if SerialReaderMonitor.KEY_TIMESTAMP in datapoint:
			if SerialReaderMonitor.KEY_CURRENT_VALUES in datapoint:
				self.informPlottersAboutCurrentValues(datapoint)
			else:
				print "ser mon: dp current values was missing that key"
		else:
			print "ser mon: dp current values was missing key timestamp"
				

	def getRawCurrentDatapoint(self):
		# print "ser mon: getting raw values"
		slidingWindow = []
		receivedDatapoint = False
		while not receivedDatapoint:
			newchar = self.serialDevice.readSingleChar()
			if newchar is None:
				return None
			slidingWindow.append(ord(newchar))
			if len(slidingWindow) > 26:
				slidingWindow = slidingWindow[1:]
			if len(slidingWindow) == 26:
				if slidingWindow[0] == 255 and slidingWindow[-1] == 255:
					slidingWindow = slidingWindow[1:-1]
					buffer = array.array('B', slidingWindow)
					floats = np.frombuffer(buffer, dtype=np.float32)
					if np.sum(floats) < 50:
						print "ser mon: np.sum(floats) < 50. returning none"
						return None
					return floats
	
	def informPlottersAboutCurrentValues(self, datapointCurrent):
		for element in self.plotterList:
			element.bufferNewCurrentDatapoint(datapointCurrent)

