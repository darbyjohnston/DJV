//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

//! \file djvGlslTestOp.cpp

#include <djvGlslTestOp.h>

#include <djvOpenGlImage.h>

djvGlslTestOp::~djvGlslTestOp()
{}

void djvGlslTestOp::begin()
{
    //DJV_DEBUG("djvGlslTestOp::begin");

    DJV_DEBUG_OPEN_GL(glPushAttrib(
        GL_VIEWPORT_BIT |
        GL_CURRENT_BIT |
        GL_ENABLE_BIT |
        GL_TRANSFORM_BIT |
        GL_PIXEL_MODE_BIT |
        GL_TEXTURE_BIT));
}

void djvGlslTestOp::end()
{
    //DJV_DEBUG("djvGlslTestOp::end");

    glPopAttrib();
    djvGlslTestShader::unbind();
}

djvGlslTestNullOp::djvGlslTestNullOp() :
    _init(false)
{}

namespace
{

const QString src =
"uniform sampler2DRect texture;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = texture2DRect(texture, gl_TexCoord[0].st);\n"
"}\n";

}

void djvGlslTestNullOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("djvGlslTestNullOp::render");
    //DJV_DEBUG_PRINT("in = " << in);

    // Initialize.

    begin();

    _texture.init(in);

    if (! _init)
    {
        //DJV_DEBUG_PRINT("init");

        _shader.init(src);
    }

    // Render.

    _shader.bind();

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(_shader.program(), "texture"), 0);
    _texture.bind();

    const djvPixelDataInfo & info = in.info();
    djvOpenGlUtil::ortho(info.size);
    glViewport(0, 0, info.size.x, info.size.y);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    djvGlslTestUtil::quad(info);

    end();
}

