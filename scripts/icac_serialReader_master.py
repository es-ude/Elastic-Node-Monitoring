#!/usr/bin/env python
import time
import array
import threading
import numpy as np

# from icac_performancePlotter import PerformanceMonitor
from lib_serialCharReader import SerialCharReader

class SerialReaderMaster(threading.Thread):

	MSG_BATCH_DURATION = 1
	MSG_EXECUTION_SWITCH = 2

	MSG_EXEC_FPGA_TYPE = 0
	MSG_EXEC_MCU_TYPE = 1
	MSG_EXEC_FPGA_TYPE_LARGER = 2
	MSG_EXEC_MCU_TYPE_LARGER = 3

	EXEC_FPGA_TYPE_STR = "FPGA"
	EXEC_MCU_TYPE_STR = "MCU"
	EXEC_FPGA_TYPE_LARGER_STR = "FPGA_LARGER"
	EXEC_MCU_TYPE_LARGER_STR = "MCU_LARGER"

	KEY_TIMESTAMP = "timestamp"
	KEY_BATCH_DURATION = "duration"

	serialDevice = None
	plotterList = []

	def __init__(self, port, baud):
		threading.Thread.__init__(self)
		# self.perfMonitor = perfMonitor
		self.finished = False
		self.port = port
		self.baud = baud
	
	def registerPlotter(self, plotterObject):
		if hasattr(plotterObject, "bufferNewBatchDuration"):
			if hasattr(plotterObject, "switchExecutionType"):
				self.plotterList.append(plotterObject)
	
	def run(self):
		self.initialise()
		while not self.finished:
			self.processMessage()
		print "ser mas: i got out of endless run loop"
	
	def stopThread(self):
		self.finished = True

	def initialise(self):
		self.serialDevice = SerialCharReader(self.port, self.baud)
		self.serialDevice.openSerial()
	
	#add new messages here
	def processMessage(self):
		messageType = self.detectMessageHeader()
		
		if messageType == SerialReaderMaster.MSG_BATCH_DURATION:
			datapoint = self.readBatchDurationMessage()
			if datapoint is not None:
				self.informPlottersAboutDuration(datapoint)
		
		if messageType == SerialReaderMaster.MSG_EXECUTION_SWITCH:
			executionType = self.readExecutionTypeSwitchMessage()
			if executionType is not None:
				self.informPlotterAboutExecutionSwitch(executionType)
			else:
				print "ser mas: execution type was read as None"

	def informPlottersAboutDuration(self, datapoint):
		for element in self.plotterList:
			element.bufferNewBatchDuration(datapoint)

	def informPlotterAboutExecutionSwitch(self, executionType):
		for element in self.plotterList:
			element.switchExecutionType(executionType)

	#Message handler
	def detectMessageHeader(self):
		slidingWindow = []
		foundHeader = False
		while not foundHeader:
			slidingWindow.append(ord(self.serialDevice.readSingleChar()))
			if len(slidingWindow) > 3:
				slidingWindow = slidingWindow[1:]
				# print "ser mas: sliding"
			if len(slidingWindow) == 3:
				if slidingWindow[0] == slidingWindow[1] == 255:
					print "ser mas: found header"
					foundHeader = True
		return slidingWindow[2]

	def readBatchDurationMessage(self):
		receivedBytes = []
		for i in range(4):
			receivedBytes.append(ord(self.serialDevice.readSingleChar()))
		buffer = array.array('B', receivedBytes)
		now = time.time()
		duration = np.frombuffer(buffer, dtype=np.float32)
		duration = duration[0]
		datapoint = {SerialReaderMaster.KEY_TIMESTAMP: now, SerialReaderMaster.KEY_BATCH_DURATION: duration}
		print datapoint
		if SerialReaderMaster.KEY_TIMESTAMP in datapoint:
			if SerialReaderMaster.KEY_BATCH_DURATION in datapoint:
				print "ser mas: received duration "
				print datapoint[SerialReaderMaster.KEY_BATCH_DURATION]
				return datapoint
			else:
				print "ser mas: datapoint duration was somehow broken"
		else:
			print "ser mas: datapoint duration was somehow broken"
		return None

	def readExecutionTypeSwitchMessage(self):
		receivedByte = ord(self.serialDevice.readSingleChar())
		print "ser mas: execution switch byte"
		print receivedByte
		if receivedByte == SerialReaderMaster.MSG_EXEC_FPGA_TYPE:
			return SerialReaderMaster.EXEC_FPGA_TYPE_STR
		elif receivedByte == SerialReaderMaster.MSG_EXEC_FPGA_TYPE_LARGER:
			return SerialReaderMaster.EXEC_FPGA_TYPE_LARGER_STR
		elif receivedByte == SerialReaderMaster.MSG_EXEC_MCU_TYPE:
			return SerialReaderMaster.EXEC_MCU_TYPE_STR
		elif receivedByte == SerialReaderMaster.MSG_EXEC_MCU_TYPE_LARGER:
			return SerialReaderMaster.EXEC_MCU_TYPE_LARGER_STR
		else:
			print "ser mas: read execution type was wrong"
			return None
