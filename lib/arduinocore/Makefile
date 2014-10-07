include ../../Makefile.common

# Includes
CFLAGS += -Isrc 

CXXFLAGS += -Isrc

# Sources
C_SRC = src/WInterrupts.c \
src/wiring.c \
src/wiring_analog.c \
src/wiring_digital.c \
src/wiring_pulse.c \
src/wiring_shift.c 

CPP_SRC = src/CDC.cpp \
src/HID.cpp \
src/HardwareSerial.cpp \
src/IPAddress.cpp \
src/Print.cpp \
src/Stream.cpp \
src/Tone.cpp \
src/USBCore.cpp \
src/WMath.cpp \
src/WString.cpp \
src/new.cpp 

OBJS := $(C_SRC:.c=.o) $(CPP_SRC:.cpp=.o)
LIBNAME = libarduinocore.a

all: $(LIBNAME)
	$(SIZE) $(LIBNAME)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

$(LIBNAME): $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBNAME)
	