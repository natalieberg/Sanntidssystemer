CC = /home/student/Desktop/git/Sanntidssystemer/Toolchain/buildroot-avr32-v3.0.0_TTK4147/output/staging/usr/bin/avr32-linux-gcc
# program executable name
TARGET = miniproject
# compiler flags
CFLAGS = -g -Wall -lpthread
# linker flags
LDFLAGS = -g -lpthread
# list of sources
SOURCES = $(shell find -name "*.c")
# default rule, to compile everything
all: $(TARGET)
# define object files
OBJECTS = $(SOURCES:.c=.o)
# link programs
$(TARGET): $(OBJECTS)
	 $(CC) -o $@ $^ $(LDFLAGS)
# compile
%.o : %.c
	$(CC) -c -o $@ $< $(CFLAGS)
# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)
