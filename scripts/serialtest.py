import serial
import time

print "Opening serial port ..."
ser = serial.Serial('/dev/ttyUSB0', 38400, timeout=1)
print "waiting 2 seconds ..."
time.sleep(2)

ser.write("\\0101FF0000\n") # red
time.sleep(.1)
ser.flush()

ser.write("\\010200FF00\n") # green
time.sleep(.1)
ser.flush()

ser.write("\\01034B0082\n")
time.sleep(.1)
ser.flush()

ser.write("\\01044B0082\n")
time.sleep(.1)
ser.flush()

ser.write("\\0105006400\n")
time.sleep(.1)
ser.flush()

#ser.flushInput()
print(ser.read(1000))


ser.write("\\01061E90FF\n") # dodger blue
time.sleep(.1)
ser.flush()


ser.write("\\0107FFA500\n") # orange
time.sleep(.1)
ser.flush()

ser.write("\\0108FFFF00\n") # yellow
time.sleep(.1)
ser.flush()

ser.write("\\0109FFC0CB\n") # orange
time.sleep(.1)
ser.flush()

#time.sleep(.1)
#ser.flushOutput()
#print("delay")
#print(ser.read(300))
#time.sleep(1)
#ser.write("\\0107FFA500\n") # orange
#time.sleep(.1)
#ser.flush()


#time.sleep(.1)
#ser.flushOutput()
#ser.write("\\02\n") # update display
#ser.flush()
#time.sleep(1)

#ser.write("\\0202\n") # update display
print(ser.read(1000))
ser.close()

