CXX = g++
SRC = $(wildcard src/*.cpp)
SHADERS = $(wildcard src/shaders/*.glsl)
OBJ = $(SRC:.cpp=.o)
BINARY_DIR = bin
OPTIMIZATION = -O3

LDLIBS = -lGL -lglfw -lGLEW -lassimp
LDFLAGS := $(shell pkg-config --cflags opencv) $(LDFLAGS)
LFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgproc -laruco

all:$(BINARY_DIR)/shaders $(BINARY_DIR)/runme

src/%.o: src/%.cpp
	$(CXX) $(LDFLAGS) -std=c++11 -Wall $(OPTIMIZATION) -o $@ -c $<

$(BINARY_DIR)/shaders: $(SHADERS)
	@mkdir -p bin/
	cp -r src/shaders bin/

$(BINARY_DIR)/runme: $(OBJ)
	@mkdir -p bin/
	$(CXX) $(LDFLAGS) -Wall $(OPTIMIZATION) -o $@ $^ $(LFLAGS) $(LDLIBS)


.PHONY: clean
clean:
	rm -rf $(OBJ) $(BINARY_DIR)
