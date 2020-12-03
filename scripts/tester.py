#!/usr/bin/env python

import threading
import time
import Queue


class MyClass:

	def update(self):
		print "i update"


if __name__ == '__main__':
	tester = MyClass()
	hasAttribute = hasattr(tester, "update")
	print hasAttribute

	hasAttribute = hasattr(tester, "notUpdate")
	print hasAttribute

# currentValues = []

# for i in range(10):
# 	datapoint = {"timestamp" : time.time(), "current" : 200}
# 	currentValues.append(datapoint)
# 	time.sleep(1)

# now = time.time()
# timeperiod = 4.0
# print now - timeperiod
# relevantCurrentValues = []

# for i in range(len(currentValues)):
# 	if currentValues[i]["timestamp"] > (now -timeperiod):
# 		relevantCurrentValues.append(currentValues[i])

# print len(relevantCurrentValues)
# print relevantCurrentValues









# #!/usr/bin/env python
# import time
# import threading

# class Printer(threading.Thread):
# 	#Object variables
# 	somelist = []
# 	newElementAvailable = False
# 	newElementList = []
# 	finished = False

# 	#methods
# 	def __init__(self, lock):
# 		threading.Thread.__init__(self)
# 		self.lock = lock

# 	def run(self):
# 		while not self.finished:
# 			if self.newElementAvailable:
# 				self.lock.acquire()
# 				listCopy = list(self.newElementList)
# 				self.newElementList = []
# 				self.newElementAvailable = False
# 				self.lock.release()

# 				for element in listCopy:
# 					self.somelist.append(element)
# 				print "added elements:"
# 				print self.somelist
# 			time.sleep(0.5)

# 	def updateElement(self, datapoint):
# 		self.newElementList.append(datapoint)
# 		self.newElementAvailable = True

# 	def stopThread(self):
# 		self.finished = True

# ######################################

# class SerialReader(threading.Thread):
# 	#Object variables
# 	finished = False

# 	#methods
# 	def __init__(self, lock, printer):
# 		threading.Thread.__init__(self)
# 		self.printer = printer
# 		self.lock = lock
	
# 	def run(self):
# 		dummyDataList = [2] * 1000
# 		for element in dummyDataList:
# 			print "new round"
# 			self.lock.acquire()
# 			self.printer.updateElement(element)
# 			self.lock.release()
# 			time.sleep(1)
# 			if self.finished:
# 				break
# 		self.printer.stopThread()

# 	def stopThread(self):
# 		self.finished = True

# if __name__=="__main__":
# 	lock=threading.Lock()
# 	thePrinter = Printer(lock)
# 	thePrinter.daemon = True
	
# 	theReader = SerialReader(lock, thePrinter)
# 	theReader.daemon = True

# 	thePrinter.start()
# 	theReader.start()

# 	try:
# 		while True:
# 			time.sleep(.1)

# 	except KeyboardInterrupt:
# 		# print "attempting to close threads. Max wait =",max(d1,d2)
# 		# run_event.clear()
# 		thePrinter.stopThread()
# 		theReader.stopThread()
# 		thePrinter.join()
# 		theReader.join()
# 		print "threads successfully closed"
