#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev
# Mac OS X:
#   brew install sdl2
# MSYS2:
#   pacman -S mingw-w64-i686-SDL
#

CXX = g++
CC = gcc
#CXX = clang++

BUILD_TARGET = Sys_Mon
BUILD_DIR = build
EXE = $(addprefix $(BUILD_DIR)/, $(basename $(notdir $(BUILD_TARGET))))
SOURCES = main.cpp
SOURCES += imgui/opengl3/imgui_impl_sdl.cpp imgui/opengl3/imgui_impl_opengl3.cpp
SOURCES += imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp
SOURCES += src/format.cpp src/process.cpp src/system.cpp src/processor.cpp src/logger.cpp
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -Iimgui/ -Iimgui/opengl3/ -Isrc/ -Iinclude/ 
CXXFLAGS += -g -Wall -Wformat 
LIBS = -lpthread

##---------------------------------------------------------------------
## OPENGL LOADER
##---------------------------------------------------------------------

## Using OpenGL loader: gl3w [default]
SOURCES += imgui/libs/gl3w/GL/gl3w.c
CXXFLAGS += -Iimgui/libs/gl3w -DIMGUI_IMPL_OPENGL_LOADER_GL3W

## Using OpenGL loader: glew
## (This assumes a system-wide installation)
# CXXFLAGS += -lGLEW -DIMGUI_IMPL_OPENGL_LOADER_GLEW

## Using OpenGL loader: glad
# SOURCES += ../libs/glad/src/glad.c
# CXXFLAGS += -I../libs/glad/include -DIMGUI_IMPL_OPENGL_LOADER_GLAD

## Using OpenGL loader: glbinding
## This assumes a system-wide installation
## of either version 3.0.0 (or newer)
# CXXFLAGS += -lglbinding -DIMGUI_IMPL_OPENGL_LOADER_GLBINDING3
## or the older version 2.x
# CXXFLAGS += -lglbinding -DIMGUI_IMPL_OPENGL_LOADER_GLBINDING2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL -ldl `sdl2-config --libs`

	CXXFLAGS += `sdl2-config --cflags`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo `sdl2-config --libs`
	LIBS += -L/usr/local/lib -L/opt/local/lib

	CXXFLAGS += `sdl2-config --cflags`
	CXXFLAGS += -I/usr/local/include -I/opt/local/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
   ECHO_MESSAGE = "MinGW"
   LIBS += -lgdi32 -lopengl32 -limm32 `pkg-config --static --libs sdl2`

   CXXFLAGS += `pkg-config --cflags sdl2`
   CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

$(BUILD_DIR)/%.o:src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o:imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o:imgui/opengl3/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o:imgui/libs/gl3w/GL/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(BUILD_DIR)/%.o:imgui/libs/glad/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)
	rm -r $(BUILD_DIR)

setup:
	mkdir $(BUILD_DIR)