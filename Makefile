
# Simple makefile for OpenCL
CFLAGS = -std=c++11 -Wall -g
LIBS   = -lOpenCL
MAGICKFLAGS = `Magick++-config --cxxflags --cppflags` `Magick++-config --ldflags --libs`
CC     = g++ $(CFLAGS) $(LIBS) $(MAGICKFLAGS)

all : subtract

image.o : image.cpp image.h
	$(CC) -c image.cpp
main.o: main.cpp
	$(CC) -c main.cpp

subtract: image.o main.o
	$(CC) -o $@ $^

valgrind: subtract
	valgrind --leak-check=full ./subtract 
clean:
	rm -f subtract *.o
run: subtract
	@./subtract
