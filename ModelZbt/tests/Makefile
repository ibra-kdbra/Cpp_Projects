#
# tests/Makefile
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

CFLAGS = -I ../ -ansi -pedantic -Wall -Wextra
LIBS =

SRCS = $(wildcard *.c)
BINS = $(SRCS:.c=)

all: $(BINS) testC++

test01: test01.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS) -lassimp

test04: test04.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS) -llua

# stb_image is not ANSI C89 conformant and it is not dependency free
test0D: test0D.c
	$(CC) -I ../ -std=c99 -pedantic -Wall -Wextra $< -o $@ -lm

testC++: test02.c
	g++ --std=c++11 -I ../ -pedantic -Wall -Wextra $< -o $@

%: %.c ../m3d.h
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

clean:
	rm $(BINS) testC++ 2>/dev/null || true
