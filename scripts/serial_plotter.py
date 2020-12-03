from matplotlib import pyplot as pp
from matplotlib import dates as mpldates
import time
import warnings 
import numpy as np
from datetime import datetime 
from measurement import CurrentMeasurement
import Queue 

LIMIT = 500
BATCH_LIMIT = 1000
GRAPH_TIME_LIMIT = 30 
HIDE_TOTAL = False
HIDE_MONITOR = False
CURRENT_SENSE_F = 12.
PLOT_TARGETS = ["power", "latency"]
FULL_LEGEND = ['Total', 'Monitor', 'Wireless', 'MCU', 'FPGA int', 'FPGA aux']
LATENCY_LEGEND = ['Elastic Node', 'Offloading']
ENERGY_LEGEND = ['Elastic Node', 'Offloading', 'Offloading without FPGA']
AVERAGE_LATENCY = False
THROUGHPUT = True
BLOCK_SIZE_LOCAL = 1000
BLOCK_SIZE_REMOTE = 50


class SerialPlotter:
	# current_graphdata = [list() for i in range(5)]
	current_graphdata = list()
	current_xs = list()

	latency_graphdata = list()
	energy_graphdata = list()
	latency_xs = list()

	latencyFig = None
	currentFig = None
		
	latencyTarget = None
	currentTarget = None
	energyTarget = None

	current_latestX = 0
	latency_latestX = 0

	updating = False
	title = "Elastic Node"
	maxCurrent = 0
	maxLatency = 0

	styles = ['r', 'b', 'g', 'm', 'k', 'c']

	hide_wireless = False

	dateFormat = mpldates.DateFormatter('%H:%M:%S')
	majorticks = mpldates.SecondLocator(interval=5)
	minorticks = mpldates.SecondLocator()

	currentMeasurementsQueue = Queue.Queue()

	def __init__(this):
		print 'Created plotter...'
		# pp.figure()
		# this.drawExtras()
		this.maxLatency = this.maxCurrent = 0

	def drawExtras(this, target=None, targetName=None):
		if target is None:
			pp.ion()
			pp.grid()
			# if latency:
			warnings.warn("targets not defined")
			if True:
				pp.ylabel('Latency (in s)')
				pp.xlabel('Sample number')
			else: 
			# else:
				pp.ylabel('Current consumption (in mA)')
				pp.xlabel('Time (in s)')

			# pp.ylabel('Power consumption (in mW)')
			pp.title(this.title)
		else:
			pp.ion()
			target.grid()


			if targetName == "power":
				target.set_xlabel('Time (in s)')
				target.set_ylabel('Power consumption (in mW)')
				target.set_title("Power of Components")
			elif targetName == "latency":
				if THROUGHPUT:
					target.set_ylabel('Average throughput (in training runs/second)')
					target.set_xlabel('Time')
					target.set_title('Throughput')
				else:
					target.set_ylabel('Latency (in ms)')
					target.set_xlabel('Batch number')
					target.set_title('Batch Latency')

				target.xaxis.set_major_locator(this.majorticks)
				target.xaxis.set_major_formatter(this.dateFormat)
				target.xaxis.set_minor_locator(this.minorticks)
			elif targetName == "energy":
				target.set_ylabel('Energy consumed per training run (in mJ/training run)')
				target.set_xlabel('Time')
				target.set_title('Energy Consumption')


				target.xaxis.set_major_locator(this.majorticks)
				target.xaxis.set_major_formatter(this.dateFormat)
				target.xaxis.set_minor_locator(this.minorticks)

			elif targetName == "currentMeasurement":
				target.set_xlabel('Time')
				target.set_ylabel('Current consumption (in mA)')
				target.set_title("Power of Components")

				target.xaxis.set_major_locator(this.majorticks)
				target.xaxis.set_major_formatter(this.dateFormat)
				target.xaxis.set_minor_locator(this.minorticks)
			# else:
			# pp.ylabel('Power consumption (in mW)')

	# return the currently averaged values
	averages = np.zeros((5,))
	lastAverages = np.array	(averages)
	currentSamples = 0
	def averageCurrent(this):
		# if np.sum(this.averages) == 0:
		# 	# print 'measurements available...'
		output = this.lastAverages
		# this.lastAverages = None # only use it once 

		return output
		# else:
		# 	return this.lastAverages
			# output = this.averages / this.currentSamples
			# clear for next time (also used for the first time)
			# print 'output', output
			

			# this.averages *= 0
			# this.currentSamples = 0;
			# return output

	def updateAverageCurrent(this, datapoint):
		this.averages += np.array(datapoint)
		this.lastAverages = np.array(datapoint)
		this.currentSamples += 1

	def clearAverageCurrent(this):
		this.lastAverages = None
		this.currentSamples = 0
		this.averages *= 0

	def queueCurrentMeasurement(this, m):
		this.currentMeasurementsQueue.put(m)

	def plotQueuedCurrentMeasurements(this):
		try:
			while not this.currentMeasurementsQueue.empty():
				m = this.currentMeasurementsQueue.get()

						# if True:
				this.updating = True



				# if m is not CurrentMeasurement:
				# 	warnings.warn('datapoint incorrect format')
				# 	print 'incorrect format'

				# 	return False

				# # convert to power
				# datapoint = np.array(datapoint) * np.array([5, 3.3, 3.3, 3.3, 3.3, 1.2])
				datapoint = m.datapoint()
				# combine aux and internal fpga into one 
				# datapoint[-2] += datapoint[-1]
				# datapoint = datapoint[:-1]
				
				# this.updateAverageCurrent(datapoint)

				this.current_graphdata.append(datapoint)
				if len(this.current_graphdata) > LIMIT:
					this.current_graphdata = this.current_graphdata[-LIMIT:]

				this.current_xs.append(m.timestamp)

				if len(this.current_xs) > LIMIT:
					this.current_xs = this.current_xs[-LIMIT:]

				this.updating = False
			return True

		except KeyboardInterrupt:
			this.updating = False
			return False

		except ValueError:
			print 'Value error in plot'
			return False

	def plotCurrent(this, datapoint):
		try:
		# if True:
			this.updating = True

			if len(datapoint) != 6:
				warnings.warn('datapoint incorrect format')
				return False

			# convert to power
			datapoint = np.array(datapoint) * np.array([5, 3.3, 3.3, 3.3, 3.3, 1.2])
			# combine aux and internal fpga into one 
			datapoint[-2] += datapoint[-1]
			datapoint = datapoint[:-1]
			
			this.updateAverageCurrent(datapoint)

			for i in range(len(datapoint)):
				this.current_graphdata[i].append(datapoint[i])
				if len(this.current_graphdata[i]) > LIMIT:
					this.current_graphdata[i] = this.current_graphdata[i][-LIMIT:]

			this.current_xs.append(this.current_latestX)
			if len(this.current_xs) > LIMIT:
				# print len(this.xs)
				this.current_xs = this.current_xs[-LIMIT:]
			# 	print len(this.xs)
			this.current_latestX += 1

			this.updating = False
			return True

		except KeyboardInterrupt:
			this.updating = False
			return False

		except ValueError:
			print 'Value error in plot'
			return False

	def redrawCurrent(this, timeout=0.05, sub=None):

		this.plotQueuedCurrentMeasurements()
		# if this.updating:
		# 	print '\tupdating!'
		# 	while this.updating:
		# 		time.sleep(0.005)
		# 		continue
		# 	print '\tdone updating!'
		#  	return

		# nothing to draw
		if len(this.current_xs) == 0:
			print 'drawing:', len(this.current_xs)
			pp.pause(timeout)

			return

		this.currentTarget.cla()
		this.drawExtras(target=this.currentTarget, targetName="currentMeasurement")
		# if sub is not None:
		# 	pp.subplot(sub)
		# 	pp.cla()
		# 	this.drawExtras()
		# else:
		# 	pp.cla()
		# 	this.drawExtras()

		# for i in range(5):
		# if not (this.hide_wireless and i == 1):
			# if not ((HIDE_TOTAL and i == 0) or (HIDE_MONITOR and i == 1)):
				# print len(this.current_xs), len(this.current_graphdata[i])
		try:
			# pp.plot(np.array(this.current_xs) / CURRENT_SENSE_F, this.current_graphdata[i], this.styles[i])
			pp.plot(this.current_xs, this.current_graphdata)
		except ValueError:
			print "Could not plot data:"

			print np.shape(np.array(this.current_xs)), np.shape(this.current_graphdata)
		try:	
			this.maxCurrent = np.max([this.maxCurrent, np.max(this.current_graphdata)])
		except ValueError:
			print 'Cannot get max current:', this.current_graphdata[i]

		minx = np.min(this.current_xs)
		maxx = np.max(this.current_xs)
		# minx = np.min(this.current_xs) / CURRENT_SENSE_F
		# maxx = np.max(this.current_xs) / CURRENT_SENSE_F
		if minx != maxx:
			this.currentTarget.set_xlim([minx, maxx])
		if this.maxCurrent != 0:
			this.currentTarget.set_ylim([0, this.maxCurrent * 1.1])

		if this.hide_wireless:
			this.currentTarget.legend(['Total', 'MCU', 'FPGA'], loc='center right')
		else:
			legend = FULL_LEGEND
			if HIDE_TOTAL and HIDE_MONITOR: 
				legend = legend[2:]
			elif HIDE_TOTAL: 
				legend = legend[1:]
			elif HIDE_MONITOR:
				warnings.warn("HIDE_MONITOR not implemented")
			this.currentTarget.legend(legend, loc='upper left')

		if this.currentFig is not None:
			this.currentFig.canvas.draw()
		pp.pause(timeout)


	def plotLatency(this, latencyData, offloading=False):
		try:
			if offloading:
				BLOCK_SIZE = BLOCK_SIZE_REMOTE
			else:
				BLOCK_SIZE = BLOCK_SIZE_LOCAL


			if AVERAGE_LATENCY:
				datapoint = latencyData / float(BLOCK_SIZE)
			elif THROUGHPUT:
				datapoint = 1.0 / latencyData * float(BLOCK_SIZE)
			else:
				datapoint = latencyData		

			# calculate the last average power for this batch
			averageCurrents = this.averageCurrent()
			# total power is everything other than total
			if averageCurrents is None:
				totalPower = None
				totalPowerWithoutFPGA = None
			else:
				totalPower = np.sum(averageCurrents[1:])
				totalPowerWithoutFPGA = np.sum(averageCurrents[1:-1])
			this.updating = True
			energyPoint = np.zeros((3,))
			latencyPoint = np.zeros((2,))
			# print 'offloading', offloading
			
			# power in mW, time in ms
			# energy in uJ
			for i in range(len(datapoint)):
				if offloading:
					latencyPoint[0] = None
					latencyPoint[1] = datapoint[i]
					energyPoint[0] = None
					if totalPower is None:
						energyPoint[1] = None
						energyPoint[2] = None
					else:
						energyPoint[1] = totalPower / datapoint[i]
						energyPoint[2] = totalPowerWithoutFPGA / datapoint[i]
				else:
					latencyPoint[0] = datapoint[i]
					latencyPoint[1] = None
					if totalPower is None:
						energyPoint[0] = None
					else:
						energyPoint[0] = totalPower / datapoint[i]
					energyPoint[1] = None
					energyPoint[2] = None

				this.latency_graphdata.append(np.array(latencyPoint))
				this.energy_graphdata.append(np.array(energyPoint))
				# this.latency_xs.append(this.latency_latestX)
				this.latency_xs.append(datetime.now())
				this.latency_latestX += 1

			for i in range(len(this.latency_xs)):
				if ((datetime.now() - this.latency_xs[i])).total_seconds() < GRAPH_TIME_LIMIT:
					break
			# prune any older than that
			if i > 0:
				this.latency_graphdata = this.latency_graphdata[i:]
				this.energy_graphdata = this.energy_graphdata[i:]	
				this.latency_xs = this.latency_xs[i:]
			
			# if len(this.latency_graphdata) > BATCH_LIMIT:
			# 	this.latency_graphdata = this.latency_graphdata[-BATCH_LIMIT:]
			# 	this.energy_graphdata = this.energy_graphdata[-BATCH_LIMIT:]

			# if len(this.latency_xs) > BATCH_LIMIT:
			# 	this.latency_xs = this.latency_xs[-BATCH_LIMIT:]

			this.updating = False
			return True

		except KeyboardInterrupt:
			this.updating = False
			return False

		except ValueError:
			print 'Value error in plot'
			return False

	def redrawLatency(this, timeout=0.05):

		if this.updating: return

		if len(this.latency_xs) == 0:
			# print 'no data yet'
			return

		this.latencyTarget.cla()
		this.drawExtras(target=this.latencyTarget, targetName="latency")
		this.energyTarget.cla()
		this.drawExtras(target=this.energyTarget, targetName="energy")
		# if sub is not None:
		# 	pp.subplot(sub)
		# 	pp.subplot.cla()
		# 	this.drawExtras()
		# else:
		# 	pp.cla()
		# 	this.drawExtras()

		# this.latencyTarget.plot(np.array(this.latency_xs), np.array(this.latency_graphdata))
		try:
			this.latencyTarget.semilogy(np.array(this.latency_xs), np.array(this.latency_graphdata))
		except ValueError:
			print "\tCould not plot latency data:"

			print np.shape(np.array(this.latency_xs)), np.shape(this.latency_graphdata)
			print np.shape(np.array(this.latency_xs)), np.shape(this.energy_graphdata)

			print np.array(this.latency_graphdata)
		try:
			this.energyTarget.semilogy(np.array(this.latency_xs), np.array(this.energy_graphdata))
		except ValueError:
			print "\tCould not plot energy data:"

			print np.shape(np.array(this.latency_xs)), np.shape(this.latency_graphdata)
			print np.shape(np.array(this.latency_xs)), np.shape(this.energy_graphdata)

			print np.array(this.energy_graphdata)
		this.maxLatency = np.max([this.maxLatency, np.max(np.array(this.latency_graphdata))])

		minx = np.min(this.latency_xs)
		maxx = np.max(this.latency_xs)
		# this.maxLatency = .1
		if minx != maxx:
			this.latencyTarget.set_xlim([minx, maxx])
		# if this.maxLatency != 0:
		# 	this.latencyTarget.set_ylim([0, this.maxLatency * 1.1])

		this.latencyTarget.legend(LATENCY_LEGEND, loc='upper left')
		this.energyTarget.legend(ENERGY_LEGEND, loc='upper left')

		this.maxLatency = 0

		if this.latencyFig is not None:
			this.latencyFig.canvas.draw()
		# pp.pause(timeout)

	def subplotTargets(this, subplots=True, subfigures=True):
		if subfigures:


			# f = pp.Figure(figsize=(20,10), dpi=100)
			# pp.figure(figsize=(20,10))
			this.latencyFig = pp.figure()
			this.currentFig = pp.figure()
			this.latencyTarget = this.latencyFig.add_subplot(2,1,1)
			this.energyTarget = this.latencyFig.add_subplot(2,1,2)
			this.currentTarget = this.currentFig.add_subplot(1,1,1)
				# this.latencyTarget = f.add_subplot(1,2,1)
				# this.currentTarget = f.add_subplot(1,2,2)
		elif subplots:
			# f = pp.Figure(figsize=(20,10), dpi=100)
			# pp.figure(figsize=(20,10))
			this.latencyTarget = pp.subplot(1,2,2)
			this.currentTarget = pp.subplot(1,2,1)
			# this.latencyTarget = f.add_subplot(1,2,1)
			# this.currentTarget = f.add_subplot(1,2,2)

		else:
			this.latencyTarget = pp
			this.currentTarget = pp


if __name__ == "__main__":
	print 'test SerialPlotter'
	plotter = SerialPlotter()

	datapoint = np.zeros((len(plotter.styles)))
	for i in range(20000):
		datapoint[0] = i
		plotter.plotCurrent(datapoint)
		plotter.plotLatency(datapoint)
		plotter.redrawCurrent()
		plotter.redrawLatency()

	time.sleep(1)

# from matplotlib import pyplot as pp
# import time
# import warnings 
# import numpy as np

# LIMIT = 100
# HIDE_TOTAL = False
# CURRENT_SENSE_F = 12.

# class SerialPlotter:
# 	graphdata = [list() for i in range(6)]
# 	xs = list()
# 	latestX = 0
# 	updating = False
# 	title = "Elastic Node Current Consumption"
# 	maxy = 0

# 	styles = ['r', 'b', 'g', 'm', 'k', 'c']

# 	hide_wireless = False

# 	def __init__(this):
# 		pp.figure()
# 		this.drawExtras()
# 		this.maxy = 0

# 	def drawExtras(this):
# 		pp.ion()
# 		pp.grid()
# 		pp.xlabel('Time (in s)')
# 		pp.ylabel('Current consumption (in mA)')
# 		# pp.ylabel('Power consumption (in mW)')
# 		pp.title(this.title)
		
# 	def plot(this, datapoint):
# 		try:
# 			this.updating = True

# 			if len(datapoint) != 6:
# 				warnings.warn('datapoint incorrect format')
# 				return False

# 			for i in range(6):
# 				this.graphdata[i].append(datapoint[i])
# 				if len(this.graphdata[i]) > LIMIT:
# 					this.graphdata[i] = this.graphdata[i][-LIMIT:]

# 			this.xs.append(this.latestX)
# 			if len(this.xs) > LIMIT:
# 				# print len(this.xs)
# 				this.xs = this.xs[-LIMIT:]
# 			# 	print len(this.xs)
# 			this.latestX += 1

# 			this.updating = False
# 			return True

# 		except KeyboardInterrupt:
# 			this.updating = False
# 			return False

# 		except ValueError:
# 			print 'Value error in plot'
# 			return False

# 	def redraw(this, timeout=0.05):

# 		if this.updating: return

# 		pp.cla()
# 		this.drawExtras()

# 		for i in range(6):
# 			if not (this.hide_wireless and i == 1):
# 				if not (HIDE_TOTAL and i == 0):
# 					pp.plot(np.array(this.xs) / CURRENT_SENSE_F, this.graphdata[i], this.styles[i])
# 					this.maxy = np.max([this.maxy, np.max(this.graphdata[i])])

# 		minx = np.min(this.xs) / CURRENT_SENSE_F
# 		maxx = np.max(this.xs) / CURRENT_SENSE_F
# 		if minx != maxx:
# 			pp.xlim([minx, maxx])
# 		if this.maxy != 0:
# 			pp.ylim([0, this.maxy * 1.1])

# 		if this.hide_wireless:
# 			pp.legend(['Total', 'MCU', 'FPGA'], loc='center right')
# 		else:
# 			legend = ['Total', 'Monitor', 'Wireless', 'MCU', 'FPGA AUX', 'FPGA INT']
# 			if HIDE_TOTAL: legend = legend[1:]
# 			pp.legend(legend, loc='upper left')


# 		pp.pause(timeout)

# if __name__ == "__main__":
# 	print 'test SerialPlotter'
# 	plotter = SerialPlotter()

# 	datapoint = np.zeros((len(plotter.styles)))
# 	for i in range(20000):
# 		datapoint[0] = i
# 		plotter.plot(datapoint)
# 		plotter.redraw()

# 	time.sleep(1)