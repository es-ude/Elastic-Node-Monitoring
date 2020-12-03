#!/usr/bin/env python3

import serial
import sys
import time

HEX = False
ser = None
trying = True

while trying:
	try:
		ser = serial.Serial(sys.argv[1], sys.argv[2])

		while True:
			line = ser.readline().decode()
			if line.endswith("\r\n"):
				line = line[:-2]
			print(line)
			# try:
			# 	usb, monitor, wireless, mcu, vccaux, vccint, newline = line.split(',')
			# 	print("usb {} monitor {} wireless {} mcu {} vccaux {} vccint {}".format(usb, monitor, wireless, mcu, vccaux, vccint))
			# except:
			# 	print("ERROR {}".format(line))
			# 	print(line.split(','))

			# sys.stdout.write(str(ser.read(1)))
			sys.stdout.flush()
	except serial.serialutil.SerialException:
		print("serial fail...")
		time.sleep(1.)
	except KeyboardInterrupt:
		trying = False
		print("key exc")
	finally:
		print('done')
		if ser is not None:
			ser.close()



# import time
# import serial
# import sys

# baud = sys.argv[2]
# if baud.endswith('UL'): baud = baud[:-2]
# baud = int(baud) 

# # print sys.argv

# ser = serial.Serial(sys.argv[1], baud)

# quit = False

# ser.write("W")
# while ser.in_waiting > 0:
# 	# print ser.in_waiting
# 	print ser.readline(), # bt = ser.read(1)[0]		while True:

# while not quit:
# 	try:
# 		ser.write("a")
# 		ser.read(1) # echo
# 		# while ser.in_waiting > 0:
# 			# print ser.in_waiting
# 		print ser.readline(), # bt = ser.read(1)[0]		while True:



# 		# time.sleep(.2)
# 			# print bt
# 			# print format(ord(bt), "02x")
# 		# print bt
# 	except serial.serialutil.SerialException:
# 		print 'serial port lost'
# 		time.sleep(.25)
# 		print 'reopening'
# 		try:
# 			ser = serial.Serial(sys.argv[1], baud)
# 		except:
# 			continue
# 	except IOError:
# 		print "IOError"
# 		continue

# ser.close
# quit = True
# print 'done'
