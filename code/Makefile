CC = g++
LFLAGS = -Wall
TARGET = test

    CFLAGS := $(shell pkg-config --cflags MagickWand) $(CFLAGS)
    LIBS := $(shell pkg-config --libs MagickWand)

runme: $(TARGET).cpp
	$(CC) -pthread -Wall -g $(TARGET).cpp $(CFLAGS) $(LFLAGS)  -lpthread   -lglut -lGL -lGLU  -lGLEW $(LIBS) -o runme

clean:
	rm ./runme
