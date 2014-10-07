import serial
import time

print "Opening serial port ..."
ser = serial.Serial('/dev/ttyUSB0', 38400, timeout=1)
print "waiting 3 seconds ..."
time.sleep(3)
ser.write("\\0101505050\n")
print(ser.read(100))
ser.close()

