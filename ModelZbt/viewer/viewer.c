/*
 * viewer/gl.c
 *
 * Copyright (C) 2022 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief Dirty workarounds for missing emscripten features
 * https://gitlab.com/bztsrc/model3d
 *
 */

#include <gl.h>

void glBitmap( GLsizei width, GLsizei height,
                                GLfloat xorig, GLfloat yorig,
                                GLfloat xmove, GLfloat ymove,
                                const GLubyte *bitmap )
{
}

void glColorMaterial( GLenum face, GLenum mode )
{
}

void glLightModeli( GLenum pname, GLint param )
{
}

void glMaterialf( GLenum face, GLenum pname, GLfloat param )
{
}

void glRasterPos2f( GLfloat x, GLfloat y )
{
}

void glPushClientAttrib( GLbitfield mask )
{
}

void glPopClientAttrib( void )
{
}
