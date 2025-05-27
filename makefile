TARGET = synthd

SRCS = main.cpp sample.cpp sample_buffer.cpp

CXX = g++

INCLUDE_DIR ?= /opt/homebrew/opt/portaudio/include -I/opt/homebrew/include
LIB_DIR ?= /opt/homebrew/opt/portaudio/lib -L/opt/homebrew/lib

CXXFLAGS = -std=c++17 -I$(INCLUDE_DIR)

LDFLAGS = -L$(LIB_DIR) -lportaudio -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)
