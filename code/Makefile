CC = g++
LFLAGS = -Wall
TARGET = temp

runme: $(TARGET).cpp
	$(CC) -Wall -g $(TARGET).cpp  -lGL -lGLU  -lglfw  -lSOIL -lGLEW -lassimp -o runme

clean:
	rm ./runme
