#!/usr/bin/env python
import sys
import time
import threading
import numpy as np
from itertools import repeat
from matplotlib import pyplot as pp
from icac_serialReader_master import SerialReaderMaster
from icac_serialReader_monitor import SerialReaderMonitor

import Queue

# the Energy per patch consists of the electric current measurements "over" the time it took to finish a batch

# we continuously receive current samples.
# we queue all samples with a system timestamp. Maximum 1000
# we receive a time period for the batch execution

# we take the last x samples that are younger than time period.timestamp - timeperiod.value
# calculate the sample rate: 

# sample rate = Number of samples/time period batch

# calculate the power for each sample in a list

# energy consumption for a batch is then:

# 	Energy = (sample rate * sum(power list))/time period batch

# Make a bar graph out of that

# FULL_LEGEND = ['Total', 'Monitor', 'Wireless', 'MCU', 'FPGA int', 'FPGA aux']
CURRENT_SAMPLE_MAX_AMOUNT = 500
MSG_EXEC_FPGA_TYPE = 0
MSG_EXEC_MCU_TYPE = 1
YLIMES = 1000

ENERGY_PLOT_NUMBER = 57

class EnergyPerBatchMonitor:
	# a currentDatapoint is {"timestamp": time.time(), measurements}
	# where measurements is a list of 6 floats in the order of 
	# [['Total', 'Monitor', 'Wireless', 'MCU', 'FPGA int', 'FPGA aux']]

	energyPerBatchMCU = 0
	energyPerBatchFPGA = 0

	energyPerBatchMCU_larger = 0
	energyPerBatchFPGA_larger = 0
	
	totalBatchMCU = totalBatchFPGA = 0
	totalBatchMCU_larger = totalBatchFPGA_larger = 0
	
	totalEnergyMCU = totalEnergyFPGA = 0
	totalEnergyMCU_larger = totalEnergyFPGA_larger = 0

	maxYever = 0

	def __init__(self, lock):
		self.lock = lock
		self.executionType = None # "FPGA"
		self.updating = False

		self.currentMeasurementQueue = Queue.Queue()
		self.latestCurrentDatapoints = []

		self.batchDurationQueue = Queue.Queue()

		self.batchEnergyConsumptionFPGA = 0
		self.batchEnergyConsumptionMCU = 0
		self.batchEnergyConsumptionFPGA_larger = 0
		self.batchEnergyConsumptionMCU_larger = 0
		self.newDatapointAvailable = False

		# pp.figure(ENERGY_PLOT_NUMBER)
		pp.subplot(2,2,4)
		
		# self.drawExtras()

	#datapointCurrent should be a timestamped value
	def bufferNewCurrentDatapoint(self, datapointCurrent):
		# print "e plot: receiving new current Measurement"
		self.currentMeasurementQueue.put(datapointCurrent)
	
	#datapointTime should be a timestamped value
	def bufferNewBatchDuration(self, datapointTime):
		# print "e plot: receiving new batch duration"
		self.batchDurationQueue.put(datapointTime)
		self.newDatapointAvailable = True
	
	def switchExecutionType(self, typeReceived):
		print "switching type to " + typeReceived
		self.lock.acquire()
		
		self.emptyBatchDurationQueue()
		if typeReceived is None:
			self.lock.release()
			return

		self.updating = True
		self.executionType = typeReceived

		# if typeReceived == SerialReaderMaster.EXEC_MCU_TYPE_STR:
		# 	self.executionType = SerialReaderMaster.EXEC_MCU_TYPE_STR

		# elif typeReceived == SerialReaderMaster.EXEC_MCU_TYPE_LARGER_STR:
		# 	self.executionType = SerialReaderMaster.EXEC_MCU_TYPE_LARGER_STR

		# elif typeReceived == SerialReaderMaster.EXEC_FPGA_TYPE_STR:
		# 	self.executionType = SerialReaderMaster.EXEC_FPGA_TYPE_STR

		# elif typeReceived == SerialReaderMaster.EXEC_FPGA_TYPE_LARGER_STR:
		# 	self.executionType = SerialReaderMaster.EXEC_FPGA_TYPE_LARGER_STR
		# else:
		# 	print "en bat mon: switching type wrong"
		# 	self.lock.release()
		# 	return

		self.updating = False

		self.lock.release()

	def emptyBatchDurationQueue(self):
		while not self.batchDurationQueue.empty():
			self.batchDurationQueue.get(block=False)
	
	def processBufferedDatapoints(self):
		print "e bat plot: process buffer"
		self.storeBufferedCurrentDatapoints()
		if not self.newDatapointAvailable:
			return
		
		latestCopiedBatchDuration = None
		while not self.batchDurationQueue.empty():
			latestCopiedBatchDuration = self.batchDurationQueue.get()
		
		self.newDatapointAvailable = False
		self.updating = True
		self.calculateEnergy(latestCopiedBatchDuration)
		self.updating = False

	def storeBufferedCurrentDatapoints(self):
		while not self.currentMeasurementQueue.empty():
			self.latestCurrentDatapoints.append(self.currentMeasurementQueue.get())
		if len(self.latestCurrentDatapoints) > CURRENT_SAMPLE_MAX_AMOUNT:
			self.latestCurrentDatapoints = self.latestCurrentDatapoints[-CURRENT_SAMPLE_MAX_AMOUNT:]

	def calculateEnergy(self, batchDuration):
		relevantCurrentDatapoints = []
		if batchDuration is None:
			print("En bat mon: Trying to calculate energy without batch duration")
			return
		durationWindow = (batchDuration[SerialReaderMaster.KEY_TIMESTAMP] - batchDuration[SerialReaderMaster.KEY_BATCH_DURATION])
		print("En bat mon: durationwindow %f" % (time.time() - durationWindow))
		for element in self.latestCurrentDatapoints:
			if element[SerialReaderMonitor.KEY_TIMESTAMP] >= durationWindow and element[SerialReaderMonitor.KEY_TIMESTAMP] <= batchDuration[SerialReaderMaster.KEY_TIMESTAMP]:
				relevantCurrentDatapoints.append(element)

		voltages = np.array([5, 3.3, 3.3, 3.3, 3.3, 1.2])
		
		#TODO: Check with albie tomorrow
		
		sumTotalEnergy = 0
		numberOfSamples = len(relevantCurrentDatapoints)
		print "e mon: number of samples:", numberOfSamples
		lastTime = None
		# averageSampleRate = 0
		totalTime = 0
		for element in relevantCurrentDatapoints:
			sumTotalPower = 0
			datapoint = element[SerialReaderMonitor.KEY_CURRENT_VALUES]
				# print(element[SerialReaderMonitor.KEY_TIMESTAMP] - lastTime)
			powerValues = np.array(datapoint) * voltages
			#calculate the power without the total sensor
			powerValues = powerValues[1:]
			for powerValue in powerValues:
				sumTotalPower += powerValue
			print "sum total power", sumTotalPower

			if lastTime is not None:
				try:
					td = (element[SerialReaderMonitor.KEY_TIMESTAMP] - lastTime)
					totalTime += td
					# for pwrValue in powerValues:
					# 	sumTotalPower += element
					sumTotalEnergy += td * sumTotalPower
					# averageSampleRate += element[SerialReaderMonitor.KEY_TIMESTAMP] - lastTime
					# print("td %f" % td)
				except IndexError:
					print element, lastTime
					sys.exit(0)
			# else:
			# 	td =

			lastTime = element[SerialReaderMonitor.KEY_TIMESTAMP]

		print ("total energy for batch %s %f %f" % (self.executionType, sumTotalEnergy, totalTime))

		if self.executionType == "FPGA":
			self.totalBatchFPGA += 1
			self.totalEnergyFPGA += sumTotalEnergy
			self.energyPerBatchFPGA = float(self.totalEnergyFPGA) / self.totalBatchFPGA

		elif self.executionType == "MCU":
			self.totalBatchMCU += 1
			self.totalEnergyMCU += sumTotalEnergy
			self.energyPerBatchMCU = float(self.totalEnergyMCU) / self.totalBatchMCU

		elif self.executionType == "MCU_LARGER":
			self.totalBatchMCU_larger += 1
			self.totalEnergyMCU_larger += sumTotalEnergy
			self.energyPerBatchMCU_larger = float(self.totalEnergyMCU_larger) / self.totalBatchMCU_larger

		elif self.executionType == "FPGA_LARGER":
			self.totalBatchFPGA_larger += 1
			self.totalEnergyFPGA_larger += sumTotalEnergy
			self.energyPerBatchFPGA_larger = float(self.totalEnergyFPGA_larger) / self.totalBatchFPGA_larger
		else:
			print "en bat mon: Type wrong when calculating energy"
			return 


	def plot(self, timeout=0.05):
		if self.updating: 
			return
		
		# pp.figure(ENERGY_PLOT_NUMBER)
		pp.subplot(2, 2, 4)

		pp.cla()
		
		x_labels = ("MCU:50", "FPGA:50", "MCU:1000", "FPGA:1000")
		y_pos = np.arange(len(x_labels))
		energyConsumption = [self.energyPerBatchMCU, self.energyPerBatchFPGA, self.energyPerBatchMCU_larger, self.energyPerBatchFPGA_larger]
		self.maxYever = np.amax([self.energyPerBatchMCU, self.energyPerBatchFPGA, self.energyPerBatchMCU_larger, self.energyPerBatchFPGA_larger, self.maxYever])

		pp.bar(y_pos, energyConsumption, align = 'center', alpha=0.5, color=('b', 'r', 'g', 'darkorange'))
		pp.grid(axis="y")
		pp.xticks(y_pos, x_labels)
		pp.ylabel('Energy (mJ)')
		pp.title('Energy Consumption per batch')
		# pp.ylim([0, YLIMES * 1.1])
		pp.ylim([0, self.maxYever * 1.1])
		pp.pause(timeout)

