//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvGraphics/OpenGLOffscreenBuffer.h>

#include <djvCore/Debug.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        namespace
        {
            int bufferCount = 0;

        } // namespace

        struct OpenGLOffscreenBuffer::Private
        {
            PixelDataInfo info;
            GLuint        id = 0;
            GLuint        texture = 0;
            GLint         restore = 0;
        };

        OpenGLOffscreenBuffer::OpenGLOffscreenBuffer(const PixelDataInfo & info) :
            _p(new Private)
        {
            //DJV_DEBUG("OpenGLOffscreenBuffer::OpenGLOffscreenBuffer");
            //DJV_DEBUG_PRINT("info = " << info);
            //DJV_DEBUG_PRINT("buffer count = " << bufferCount);

            _p->info = info;

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

            // Create the texture.
            glFuncs->glGenTextures(1, &_p->texture);
            if (!_p->texture)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLOffscreenBuffer",
                    errorLabels()[ERROR_CREATE_TEXTURE]);
            }
            glFuncs->glBindTexture(GL_TEXTURE_2D, _p->texture);
            glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glFuncs->glTexImage2D(
                GL_TEXTURE_2D,
                0,
                OpenGL::internalFormat(_p->info.pixel),
                _p->info.size.x,
                _p->info.size.y,
                0,
                OpenGL::format(_p->info.pixel, _p->info.bgr),
                OpenGL::type(_p->info.pixel),
                0);
            glFuncs->glBindTexture(GL_TEXTURE_2D, 0);

            // Create the FBO.
            glFuncs->glGenFramebuffers(1, &_p->id);
            if (!_p->id)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLOffscreenBuffer",
                    errorLabels()[ERROR_CREATE_FBO]);
            }
            OpenGLOffscreenBufferScope scope(this);
            glFuncs->glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D,
                _p->texture,
                0);
            GLenum error = glFuncs->glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (error != GL_FRAMEBUFFER_COMPLETE)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLOffscreenBuffer",
                    errorLabels()[ERROR_INIT_FBO]);
            }
            //DJV_DEBUG_PRINT("id = " << static_cast<int>(_id));

            ++bufferCount;
        }

        OpenGLOffscreenBuffer::~OpenGLOffscreenBuffer()
        {
            //DJV_DEBUG("OpenGLOffscreenBuffer::~OpenGLOffscreenBuffer");
            //DJV_DEBUG_PRINT("id = " << static_cast<int>(_id));

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

            --bufferCount;
            //DJV_DEBUG_PRINT("buffer count = " << bufferCount);

            if (_p->id)
            {
                glFuncs->glDeleteFramebuffers(1, &_p->id);
            }
            if (_p->texture)
            {
                glFuncs->glDeleteTextures(1, &_p->texture);
            }
        }

        const PixelDataInfo & OpenGLOffscreenBuffer::info() const
        {
            return _p->info;
        }

        GLuint OpenGLOffscreenBuffer::id() const
        {
            return _p->id;
        }

        GLuint OpenGLOffscreenBuffer::texture() const
        {
            return _p->texture;
        }

        void OpenGLOffscreenBuffer::bind()
        {
            if (!_p->id)
                return;
            //DJV_DEBUG("OpenGLOffscreenBuffer::bind");
            //DJV_DEBUG_PRINT("id = " << static_cast<int>(_p->id));
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_p->restore);
            //DJV_DEBUG_PRINT("restore = " << static_cast<int>(_p->restore));
            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _p->id);
        }

        void OpenGLOffscreenBuffer::unbind()
        {
            if (!_p->id)
                return;
            //DJV_DEBUG("OpenGLOffscreenBuffer::unbind");
            //DJV_DEBUG_PRINT("id = " << static_cast<int>(_id));
            //DJV_DEBUG_PRINT("restore = " << static_cast<int>(_restore));
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _p->restore);
            _p->restore = 0;
        }

        const QStringList & OpenGLOffscreenBuffer::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::OpenGLOffscreenBuffer", "Cannot create texture") <<
                qApp->translate("djv::Graphics::OpenGLOffscreenBuffer", "Cannot initialize texture") <<
                qApp->translate("djv::Graphics::OpenGLOffscreenBuffer", "Cannot create FBO") <<
                qApp->translate("djv::Graphics::OpenGLOffscreenBuffer", "Cannot initialize FBO");
            DJV_ASSERT(ERROR_COUNT == data.count());
            return data;
        }

        OpenGLOffscreenBufferScope::OpenGLOffscreenBufferScope(OpenGLOffscreenBuffer * buffer) :
            _buffer(buffer)
        {
            _buffer->bind();
        }

        OpenGLOffscreenBufferScope::~OpenGLOffscreenBufferScope()
        {
            _buffer->unbind();
        }

    } // namespace Graphics
} // namespace djv
