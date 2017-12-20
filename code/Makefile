CC = g++
TARGET = temp
LIBS = -lGL -lglfw  -lSOIL -lGLEW -lassimp
CFLAGS := $(shell pkg-config --cflags opencv) $(CFLAGS)
LFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgproc

#-lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab

runme: $(TARGET).cpp
	$(CC) -Wall -O3 -g $(TARGET).cpp  $(CFLAGS) $(LFLAGS) $(LIBS)  -o runme

clean:
	rm ./runme

fo:
	rm out/tmp*
	rm out/nor*
