CC = gcc
CFLAGS = -O2 -fPIC -Wall

BUILD = build

.PHONY : all clean

LIBSRCS = *.cpp, *.c

BUILD_O = $(BUILD)/o

objects = main.o

main :  $(objects) CC CFLAGS main $(objects)

$(objects) : main.h

clean :
	rm -rf $(BUILD)

$(BUILD) : $(BUILD_O)

$(BUILD_O) :
	mkdir -p $@

.PHONY : all clean

