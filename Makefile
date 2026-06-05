ifeq ($(OS),Windows_NT)
    TARGET = src/minecraft_scene.exe
    CXX = g++
    CC = gcc
    CXXFLAGS = -std=c++23 -O2 -I./include -I./include/freetype2
    CFLAGS = -O2 -I./include
    LDFLAGS = -static-libgcc -static-libstdc++ -Wl,-Bstatic -lglfw3 -lfreetype -Wl,-Bdynamic -lopengl32 -lgdi32 -luser32 -lshell32
    CLEAN = del /Q src\*.o $(subst /,\,$(TARGET)) 2>NUL
else
    TARGET = src/minecraft_scene
    CXX = g++
    CC = gcc
    CXXFLAGS = -std=c++23 -O2 -I./include -I/usr/include/freetype2
    CFLAGS = -O2 -I./include
    LDFLAGS = -lglfw -lfreetype -lGL -lX11 -lpthread -lXrandr -lXi -ldl
    CLEAN = rm -f src/*.o $(TARGET)
endif

CXX_SRCS = src/main.cpp
C_SRCS = src/glad.c

CXX_OBJS = $(CXX_SRCS:.cpp=.o)
C_OBJS = $(C_SRCS:.c=.o)
OBJS = $(CXX_OBJS) $(C_OBJS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(CLEAN)