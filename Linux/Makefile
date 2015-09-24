CC = g++
CFLAGS = -fPIC -lACE

all : main

main : main.o
	$(CC) -o $(CFLAGS) main.cpp
main.o : main.cpp
	$(CC) -c $(CFLAGS) main.cpp

clean :
	rm -rf *.o *.a *.out

.PHONY : all clean

