PROJECT_NAME = LearningLightingInOpenGL
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -std=c++23
INCLUDES = -I./src
LDFLAGS_STATIC = -static-libgcc -static-libstdc++

SOURCES = src/main.cpp src/glad.c 

WIN_TARGET = src/$(PROJECT_NAME).exe
WIN_LIBS = -lglfw3 -lgdi32 -lopengl32
WIN_LDFLAGS = -static $(LDFLAGS_STATIC)

LINUX_TARGET = src/$(PROJECT_NAME)
LINUX_LIBS = -lglfw -lGL -lX11 -lpthread -lrt -lm -ldl
LINUX_LDFLAGS = $(LDFLAGS_STATIC)

ifeq ($(OS),Windows_NT)
    DEFAULT_TARGET = windows
else
    DEFAULT_TARGET = linux
endif

all: $(DEFAULT_TARGET)

windows:
	@echo "Building for Windows..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(WIN_TARGET) $(INCLUDES) $(WIN_LIBS) $(WIN_LDFLAGS)
	@echo "Successfully built $(WIN_TARGET)"

linux:
	@echo "Building for Linux..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(LINUX_TARGET) $(INCLUDES) $(LINUX_LIBS) $(LINUX_LDFLAGS)
	@echo "Successfully built $(LINUX_TARGET)"

clean:
	@echo "Cleaning up..."
	rm -f $(WIN_TARGET) $(LINUX_TARGET)