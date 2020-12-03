#!/usr/bin/env python

import sys
import time
import serial
import threading

from serial import SerialTimeoutException


class SerialCharReader:

	port = None
	baud = None
	timeout = None

	def __init__(self, port, baud, timeout=None):
		self.finished = False
		self.port = port
		self.baud = baud
		self.ser = None
		self.timeout = timeout

	def openSerial(self):
		attempts = 0
		notConnected = True
		while notConnected and attempts < 10:
			try:
				print 'Opening serial...'
				print 'port ' + str(self.port)
				print 'baud ' + str(self.baud)
				self.ser = serial.Serial(self.port, self.baud)
				self.ser.timeout = self.timeout

				self.ser.reset_input_buffer()

				notConnected = False

				print 'Serial available'
			except serial.SerialException:
				print 'Port not available'
				time.sleep(1)
				attempts += 1
			except KeyboardInterrupt:
				print 'Interrupted initialisation'
				sys.exit(0)
	
	#returns the raw char
	#should be blocking
	def readSingleChar(self):

		read = None
		try:
			read = self.ser.read(1)
			if read is None:
				print "nothing received"
				try:
					self.ser.close()
				except serial.SerialException:
					print "Port wasn't open"
				self.openSerial()

				return self.readSingleChar()
			else:
				return read
		except:
			print "Serial timeout"
			try:
				self.ser.close()
			except serial.SerialException:
				print "Port wasn't open"

			self.openSerial()
			time.sleep(2)
			return self.readSingleChar()
		# finally:
		return read

	def closeSerial(self):
		self.ser.close()