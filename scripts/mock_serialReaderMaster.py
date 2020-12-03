import time
import threading

class SerialMonitorMock(threading.Thread):

	dummyDataListFPGA = [2] * 10
	dummyDataListMCU = [1] * 20

	#this can also contain object variables
	def __init__(self, perfMonitor):
		threading.Thread.__init__(self)
		self.perfMonitor = perfMonitor
		self.finished = False

	#threading methods
	def run(self):
		while not self.finished:
			print "mocking MCU inputs"
			self.perfMonitor.switchExecutionType("MCU")
			for element in self.dummyDataListMCU:
				self.perfMonitor.bufferNewBatchExecutionTimePeriod(element)
				time.sleep(.5)
			print "mocking FPGA inputs"
			self.perfMonitor.switchExecutionType("FPGA")
			for element in self.dummyDataListFPGA:
				self.perfMonitor.bufferNewBatchExecutionTimePeriod(element)
				time.sleep(.5)

	def stopThread(self):
		self.finished = True
