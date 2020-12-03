#!/usr/bin/env python

import serial
import sys
import array
import numpy as np




#Debug raw
# try:
# 	while True:
# 		# print ser.readline()
# 		valueReceived = ord(ser.read(1))
# 		if valueReceived == 10:
# 			sys.stdout.write('\n')
# 		else:
# 			sys.stdout.write(str(valueReceived) + '\t')
# 		# sys.stdout.write()
# 		sys.stdout.flush()
# except KeyboardInterrupt:
# 	print "key exc"
# finally:
# 	print 'done'
# 	ser.close()

def dataPoint():
	slidingWindow = []
	receivedDatapoint = False
	while not receivedDatapoint:
		slidingWindow.append(ord(ser.read(1)))
		# print slidingWindow
		if len(slidingWindow) > 26:
			slidingWindow = slidingWindow[1:]
			# print "\n"
		# print slidingWindow
		if len(slidingWindow) == 26:
			if slidingWindow[0] == 255 and slidingWindow[-1] == 255:
				slidingWindow = slidingWindow[1:-1]
				# print slidingWindow
				buffer = array.array('B', slidingWindow)
				floats = np.frombuffer(buffer, dtype=np.float32)
				# print floats
				# print "returning"
				return floats

def printRawReceivedByte():
	print ord(ser.read(1))
	print " "

if __name__ == '__main__':
	ser = serial.Serial(sys.argv[1], sys.argv[2])
	try:
		while True:
			# print dataPoint()
			printRawReceivedByte()

	except KeyboardInterrupt:
		print "key exc"

	finally:
		print 'done'
		ser.close()