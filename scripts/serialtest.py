import serial
import time

print "Opening serial port ..."
ser = serial.Serial('/dev/ttyUSB0', 38400, timeout=1)
print "waiting 2 seconds ..."
time.sleep(2)
ser.write("\\01014B0082\n")
print(ser.read(100))
ser.close()

