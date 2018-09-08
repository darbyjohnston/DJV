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

#include <djvGLSLTestAbstractOp.h>

#include <djvGLSLTestUtil.h>

using namespace gl;

//------------------------------------------------------------------------------
// djvGLSLTestAbstractOp
//------------------------------------------------------------------------------

djvGLSLTestAbstractOp::djvGLSLTestAbstractOp(djvGLSLTestContext * context) :
    _context(context)
{}

djvGLSLTestAbstractOp::~djvGLSLTestAbstractOp()
{}

djvGLSLTestContext * djvGLSLTestAbstractOp::context() const
{
    return _context;
}

void djvGLSLTestAbstractOp::begin()
{
    //DJV_DEBUG("djvGLSLTestAbstractOp::begin");
    DJV_DEBUG_OPEN_GL(glPushAttrib(
        GL_VIEWPORT_BIT |
        GL_CURRENT_BIT |
        GL_ENABLE_BIT |
        GL_TRANSFORM_BIT |
        GL_PIXEL_MODE_BIT |
        GL_TEXTURE_BIT));
}

void djvGLSLTestAbstractOp::end()
{
    //DJV_DEBUG("djvGLSLTestAbstractOp::end");
    glPopAttrib();
    glUseProgram(0);
}

//------------------------------------------------------------------------------
// djvGLSLTestAbstractOpWidget
//------------------------------------------------------------------------------

djvGLSLTestAbstractOpWidget::djvGLSLTestAbstractOpWidget(
    djvGLSLTestContext * context) :
    _context(context)
{}

djvGLSLTestAbstractOpWidget::~djvGLSLTestAbstractOpWidget()
{}

djvGLSLTestContext * djvGLSLTestAbstractOpWidget::context() const
{
    return _context;
}

//------------------------------------------------------------------------------
// djvGLSLTestAbstractOpFactory
//------------------------------------------------------------------------------

djvGLSLTestAbstractOpFactory::djvGLSLTestAbstractOpFactory(
    djvGLSLTestContext * context) :
    _context(context)
{}

djvGLSLTestAbstractOpFactory::~djvGLSLTestAbstractOpFactory()
{}

djvGLSLTestAbstractOpWidget * djvGLSLTestAbstractOpFactory::createWidget(
    djvGLSLTestAbstractOp *) const
{
    return 0;
}

djvGLSLTestContext * djvGLSLTestAbstractOpFactory::context() const
{
    return _context;
}

