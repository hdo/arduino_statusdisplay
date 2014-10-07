include ../../Makefile.common

#vpath %.c src

# Includes
CFLAGS += -Iinc 
CFLAGS += -I../../conf

# Sources
SRCS  = src/light_ws2812.c

OBJS = $(SRCS:.c=.o)
LIBNAME = libws2812.a

all: $(LIBNAME)
	$(SIZE) $(LIBNAME)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(LIBNAME): $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBNAME)
	