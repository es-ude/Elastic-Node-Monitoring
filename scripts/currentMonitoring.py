#!/usr/bin/env python
import os
import serial
import sys
import time
import glob
import time 
import string 
import warnings 
import threading
import array

# expects 6 floats with a 0xff before and after them. Plots those currently as current. 

from serial_plotter import SerialPlotter

class StandaloneDemo:
	
	def __init__(this, port, baud):
		this.port = port
		this.baud = baud

	def openSerial(this):
		attempts = 0
		notConnected = True
		while notConnected and attempts < 10:
			try:
				print 'Opening serial...'
				this.ser = serial.Serial(this.port, this.baud)
  
				this.ser.reset_input_buffer();

				notConnected = False

				print 'Serial available'
			except serial.SerialException:
				print 'Port not available'
				time.sleep(1)
				attempts += 1
			except KeyboardInterrupt:
				print 'Interrupted initialisation'
				sys.exit(0)



	def runDemo(this):
		try:
			this.openSerial()
			print "opened serial"
			this.plotter = SerialPlotter()
			print "created plotter"
			while True:
				datapoint = this.getDatapoint()
				validDatapoint = True
				for val in datapoint:
					if val > 1000.0:
						validDatapoint = False
				# print datapoint
				if validDatapoint:
					this.plotter.plot(datapoint)
					this.plotter.redraw()

		except KeyboardInterrupt:
			print "key exc"
			sys.exit(0)
		finally:
			print 'done'

	def getDatapoint(this):
		slidingWindow = []
		receivedDatapoint = False
		while not receivedDatapoint:
			slidingWindow.append(ord(this.ser.read(1)))
			if len(slidingWindow) > 26:
				slidingWindow = slidingWindow[1:]
			if len(slidingWindow) == 26:
				if slidingWindow[0] == 255 and slidingWindow[-1] == 255:
					slidingWindow = slidingWindow[1:-1]
					buffer = array.array('B', slidingWindow)
					floats = np.frombuffer(buffer, dtype=np.float32)
					return floats

if __name__ == '__main__':
	demo = StandaloneDemo(sys.argv[1], sys.argv[2])
	demo.runDemo()
