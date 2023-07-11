#
# m3dview/Makefile
#
# Copyright (C) 2019 bzt (bztsrc@gitlab)
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#

# --- set according to your configuration ---
MINGWSDL = ../..
# -------------------------------------------

CFLAGS = -I ../ -ansi -pedantic -Wall -Wextra
LIBS =
SRCS = viewer.c
TARGET = m3dview

ifeq ($(DRIVER),)
ifneq ("$(wildcard /Library/Frameworks/GLFW*)","")
DRIVER = APPLE_GLFW
else
ifneq ("$(wildcard /Library/Frameworks/GLUT*)","")
DRIVER = APPLE_GLUT
else
ifneq ("$(wildcard /Library/Frameworks/SDL*)","")
DRIVER = APPLE_SDL
else
ifneq ("$(wildcard /usr/include/GLFW/glfw*.h)","")
DRIVER = MINGW_GLFW
else
ifneq ("$(wildcard /usr/include/GL/glut.h)","")
DRIVER = MINGW_GLUT
else
ifneq ("$(wildcard /usr/include/SDL2/SDL.h)","")
DRIVER = MINGW_SDL
else
ifneq ("$(wildcard $(MINGWSDL)/i686-w64-mingw32/include/SDL2/SDL.h)","")
DRIVER = MINGW_SDL
CFLAGS += -I$(MINGWSDL)/i686-w64-mingw32/include/SDL2 -L../../i686-w64-mingw32/lib
endif
endif
endif
endif
endif
endif
endif
endif

ifeq ($(DRIVER),APPLE_GLFW)
SRCS += glfw.c
CFLAGS += -I/usr/include/OpenGL -I/usr/include/GLFW -I/Library/Frameworks/OpenGL.framework/Headers -I/Library/Frameworks/GLFW.framework/Headers -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -DGL_SILENCE_DEPRECATION -Wno-comment
LIBS = -F/Library/Frameworks -F/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks -framework OpenGL -framework GLFW
endif
ifeq ($(DRIVER), APPLE_GLUT)
SRCS += glut.c
CFLAGS += -I/usr/include/OpenGL -I/usr/include/GLUT -I/Library/Frameworks/OpenGL.framework/Headers -I/Library/Frameworks/GLUT.framework/Headers -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -DGL_SILENCE_DEPRECATION -Wno-comment
LIBS = -F/Library/Frameworks -F/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks -framework OpenGL -framework GLUT
endif
ifeq ($(DRIVER), APPLE_SDL)
SRCS += sdl.c
CFLAGS += -I/usr/include/OpenGL -I/usr/include/SDL2 -I/Library/Frameworks/OpenGL.framework/Headers -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -DGL_SILENCE_DEPRECATION -Wno-comment
LIBS = -F/Library/Frameworks -F/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks -framework OpenGL -framework SDL2
endif
ifeq ($(DRIVER), MINGW_GLFW)
SRCS += glfw.c
CFLAGS += -I/usr/include/GL -I/usr/include/GLFW
LIBS = -lGL -lglfw
endif
ifeq ($(DRIVER), MINGW_GLUT)
SRCS += glut.c
CFLAGS += -I/usr/include/GL
LIBS = -lGL -lglut
endif
ifeq ($(DRIVER), MINGW_SDL)
SRCS += sdl.c
ifeq ($(PREVIEW), 1)
CFLAGS += -I/usr/include/GL -I/usr/include/SDL2 -DPREVIEW=1
LIBS = -lGL -lSDL2 -lSDL2_image -lSDL2_gfx
else
CFLAGS += -I/usr/include/GL -I/usr/include/SDL2
ifeq ("$(wildcard /usr/lib/libGL*)","")
LIBS = -lopengl32 -lcomdlg32 -lSDL2
else
LIBS = -lGL -lSDL2
endif
endif
endif
ifneq ($(PROFILING),)
CFLAGS += -g -DPROFILING
endif

all: $(TARGET)

$(TARGET): $(SRCS) viewer.h ../m3d.h
ifneq ("$(LIBS)","")
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LIBS)
else
	@echo "Unable to detect ui framework. Install glfw, glut or sdl2"
endif

clean:
	rm *.o $(TARGET) $(TARGET).exe 2>/dev/null || true
