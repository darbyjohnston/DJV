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

//! \file djvOpenGlContext.cpp

#include <djvOpenGlContext.h>

#if defined(DJV_WINDOWS)

#include <djvWglContextPrivate.h>

#elif defined(DJV_OSX)

#include <djvCglContextPrivate.h>

#else

#include <djvGlxContextPrivate.h>

#endif

//------------------------------------------------------------------------------
// djvOpenGlContext::P
//------------------------------------------------------------------------------

struct djvOpenGlContext::P
{
    QString vendor;
    QString renderer;
    QString version;
};

//------------------------------------------------------------------------------
// djvOpenGlContext
//------------------------------------------------------------------------------

djvOpenGlContext::djvOpenGlContext() throw (djvError) :
    _p(new P)
{
    //DJV_DEBUG("djvOpenGlContext::djvOpenGlContext");
}

djvOpenGlContext::~djvOpenGlContext()
{
    //DJV_DEBUG("djvOpenGlContext::~djvOpenGlContext");

    delete _p;
}

const QString & djvOpenGlContext::vendor() const
{
    return _p->vendor;
}

const QString & djvOpenGlContext::renderer() const
{
    return _p->renderer;
}

const QString & djvOpenGlContext::version() const
{
    return _p->version;
}

void djvOpenGlContext::setVendor(const QString & vendor)
{
    _p->vendor = vendor;
}

void djvOpenGlContext::setRenderer(const QString & renderer)
{
    _p->renderer = renderer;
}

void djvOpenGlContext::setVersion(const QString & version)
{
    _p->version = version;
}

//------------------------------------------------------------------------------
// djvOpenGlContextFactory
//------------------------------------------------------------------------------

djvOpenGlContextFactory::~djvOpenGlContextFactory()
{}

djvOpenGlContext * djvOpenGlContextFactory::create(bool bind) throw (djvError)
{
    djvOpenGlContext * context = 0;

#if defined(DJV_WINDOWS)

    context = new djvWglContextPrivate;

#elif defined(DJV_OSX)

    context = new djvCglContextPrivate;

#else // DJV_WINDOWS

    context = new djvGlxContextPrivate;

#endif // DJV_WINDOWS

    if (context && bind)
    {
        context->bind();
    }

    return context;
}

//------------------------------------------------------------------------------
// djvOpenGlContextScope
//------------------------------------------------------------------------------

djvOpenGlContextScope::djvOpenGlContextScope(djvOpenGlContext * context) :
    _context(context)
{
    _context->bind();
}

djvOpenGlContextScope::~djvOpenGlContextScope()
{
    _context->unbind();
}
