include ../../Makefile.common

# Sources

CPP_SRC += \
main.cpp

# Project name
PROJ_NAME=arduinows2812test
OUTPATH=build


vpath %.c src
vpath %.cpp src
vpath %.a ../../lib

# Includes
CFLAGS += -Iinc
CFLAGS += -I../../conf
CFLAGS += -I../../lib/arduinocore/src
CFLAGS += -I../../lib/ws2812/inc

CXXFLAGS += -Iinc
CXXFLAGS += -I../../conf
CXXFLAGS += -I../../lib/arduinocore/src
CXXFLAGS += -I../../lib/ws2812/inc

# Library paths
LIBPATHS  = -L../../lib/arduinocore
LIBPATHS  += -L../../lib/ws2812

# Libraries to link
LIBS  = -larduinocore
LIBS  += -lws2812

OBJS = $(CPP_SRC:.cpp=.o)

###################################################

.PHONY: lib proj

flash: all 
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

all: lib proj 
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

lib:
	$(MAKE) -C ../../lib 

proj: prepare $(OUTPATH)/$(PROJ_NAME).elf
	

$(OUTPATH)/$(PROJ_NAME).elf: $(CPP_SRC)
	mkdir -p $(OUTPATH)	
	#bash ./scripts/setbuildid.script
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBPATHS) $(LIBS)
	$(OBJCOPY) -j .text  -j .data -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	
cleanall: clean
	$(MAKE) clean -C ../../lib # Remove this line if you don't want to clean the libs as well

prepare:
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin
	
clean:
	rm -f *.o
	rm -f $(OUTPATH)/*