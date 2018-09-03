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

#include <djvGlslTestAbstractOp.h>

#include <djvGlslTestUtil.h>

using namespace gl;

//------------------------------------------------------------------------------
// djvGlslTestAbstractOp
//------------------------------------------------------------------------------

djvGlslTestAbstractOp::djvGlslTestAbstractOp(djvGlslTestContext * context) :
    _context(context)
{}

djvGlslTestAbstractOp::~djvGlslTestAbstractOp()
{}

djvGlslTestContext * djvGlslTestAbstractOp::context() const
{
    return _context;
}

void djvGlslTestAbstractOp::begin()
{
    //DJV_DEBUG("djvGlslTestAbstractOp::begin");
    DJV_DEBUG_OPEN_GL(glPushAttrib(
        GL_VIEWPORT_BIT |
        GL_CURRENT_BIT |
        GL_ENABLE_BIT |
        GL_TRANSFORM_BIT |
        GL_PIXEL_MODE_BIT |
        GL_TEXTURE_BIT));
}

void djvGlslTestAbstractOp::end()
{
    //DJV_DEBUG("djvGlslTestAbstractOp::end");
    glPopAttrib();
    glUseProgram(0);
}

//------------------------------------------------------------------------------
// djvGlslTestAbstractOpWidget
//------------------------------------------------------------------------------

djvGlslTestAbstractOpWidget::djvGlslTestAbstractOpWidget(
    djvGlslTestContext * context) :
    _context(context)
{}

djvGlslTestAbstractOpWidget::~djvGlslTestAbstractOpWidget()
{}

djvGlslTestContext * djvGlslTestAbstractOpWidget::context() const
{
    return _context;
}

//------------------------------------------------------------------------------
// djvGlslTestAbstractOpFactory
//------------------------------------------------------------------------------

djvGlslTestAbstractOpFactory::djvGlslTestAbstractOpFactory(
    djvGlslTestContext * context) :
    _context(context)
{}

djvGlslTestAbstractOpFactory::~djvGlslTestAbstractOpFactory()
{}

djvGlslTestAbstractOpWidget * djvGlslTestAbstractOpFactory::createWidget(
    djvGlslTestAbstractOp *) const
{
    return 0;
}

djvGlslTestContext * djvGlslTestAbstractOpFactory::context() const
{
    return _context;
}

