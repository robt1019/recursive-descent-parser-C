CFLAGS = `sdl2-config --cflags` -O4 -Wall -pedantic -std=c99 -lm
INCS = display.h turtle.h
TARGET = turtle
SOURCES = display.c testing.c $(TARGET).c
LIBS =  `sdl2-config --libs`
CC = gcc

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)