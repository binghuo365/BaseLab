CC = gcc
CFLAGS = -O2 -fPIC -Wall
AR = ar rc

BUILD = build

.PHONY : all clean

LIBSRCS = *.cpp, *.c

BUILD_O = $(BUILD)/o

all : test

clean :
	rm -rf $(BUILD)

$(BUILD) : $(BUILD_O)

$(BUILD_O) :
	mkdir -p $@

.PHONY : all clean

