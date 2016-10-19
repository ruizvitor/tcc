CC = g++
LFLAGS = -Wall
TARGET = test

    # CFLAGS := $(shell pkg-config --cflags MagickWand) $(CFLAGS)
    # LIBS := $(shell pkg-config --libs MagickWand)

runme: $(TARGET).cpp
	$(CC) -pthread -Wall -g $(TARGET).cpp   -lpthread   -lglut -lGL  -lSOIL -lGLU  -lGLEW -lassimp -o runme

clean:
	rm ./runme
