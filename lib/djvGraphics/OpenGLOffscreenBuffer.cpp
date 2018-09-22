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

        OpenGLOffscreenBuffer::OpenGLOffscreenBuffer(const PixelDataInfo & info) :
            _info(info)
        {
            //DJV_DEBUG("OpenGLOffscreenBuffer::OpenGLOffscreenBuffer");
            //DJV_DEBUG_PRINT("info = " << info);
            //DJV_DEBUG_PRINT("buffer count = " << bufferCount);

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

            // Create the texture.
            DJV_DEBUG_OPEN_GL(glFuncs->glGenTextures(1, &_texture));
            if (!_texture)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLOffscreenBuffer",
                    errorLabels()[ERROR_CREATE_TEXTURE]);
            }
            DJV_DEBUG_OPEN_GL(glFuncs->glBindTexture(GL_TEXTURE_2D, _texture));
            DJV_DEBUG_OPEN_GL(glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            DJV_DEBUG_OPEN_GL(glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            DJV_DEBUG_OPEN_GL(glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            DJV_DEBUG_OPEN_GL(glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GLenum format = GL_RGBA;
            if (Pixel::F16 == Pixel::type(_info.pixel))
            {
                format = GL_RGBA16F;
            }
            else if (Pixel::F32 == Pixel::type(_info.pixel))
            {
                format = GL_RGBA32F;
            }
            glFuncs->glTexImage2D(
                GL_TEXTURE_2D,
                0,
                format,
                _info.size.x,
                _info.size.y,
                0,
                OpenGLUtil::format(_info.pixel, _info.bgr),
                OpenGLUtil::type(_info.pixel),
                0);
            GLenum error = glFuncs->glGetError();
#if ! defined(DJV_OSX)
            //! \todo On OS X this error is triggered in djv_view when a new file is
            //! loaded, though it doesn't actually seem to be a problem?
            if (error != GL_NO_ERROR)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLOffscreenBuffer",
                    errorLabels()[ERROR_INIT_TEXTURE].
                    arg(OpenGLUtil::errorString(error)));
            }
#endif // DJV_OSX
            DJV_DEBUG_OPEN_GL(glFuncs->glBindTexture(GL_TEXTURE_2D, 0));

            // Create the FBO.
            DJV_DEBUG_OPEN_GL(glFuncs->glGenFramebuffers(1, &_id));
            if (!_id)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLOffscreenBuffer",
                    errorLabels()[ERROR_CREATE_FBO]);
            }
            OpenGLOffscreenBufferScope scope(this);
            DJV_DEBUG_OPEN_GL(glFuncs->glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D,
                _texture,
                0));
            error = glFuncs->glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (error != GL_FRAMEBUFFER_COMPLETE)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLOffscreenBuffer",
                    errorLabels()[ERROR_INIT_FBO].
                    arg(OpenGLUtil::errorString(error)));
            }
            //DJV_DEBUG_PRINT("id = " << static_cast<int>(_id));

            ++bufferCount;
        }

        OpenGLOffscreenBuffer::~OpenGLOffscreenBuffer()
        {
            //DJV_DEBUG("OpenGLOffscreenBuffer::~OpenGLOffscreenBuffer");
            //DJV_DEBUG_PRINT("id = " << static_cast<int>(_id));

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

            --bufferCount;
            //DJV_DEBUG_PRINT("buffer count = " << bufferCount);

            if (_id)
            {
                DJV_DEBUG_OPEN_GL(glFuncs->glDeleteFramebuffers(1, &_id));
            }
            if (_texture)
            {
                DJV_DEBUG_OPEN_GL(glFuncs->glDeleteTextures(1, &_texture));
            }
        }

        const PixelDataInfo & OpenGLOffscreenBuffer::info() const
        {
            return _info;
        }

        GLuint OpenGLOffscreenBuffer::id() const
        {
            return _id;
        }

        GLuint OpenGLOffscreenBuffer::texture() const
        {
            return _texture;
        }

        void OpenGLOffscreenBuffer::bind()
        {
            if (!_id)
                return;
            //DJV_DEBUG("OpenGLOffscreenBuffer::bind");
            //DJV_DEBUG_PRINT("id = " << static_cast<int>(_id));
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            DJV_DEBUG_OPEN_GL(glFuncs->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_restore));
            //DJV_DEBUG_PRINT("restore = " << static_cast<int>(_restore));
            DJV_DEBUG_OPEN_GL(glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _id));
        }

        void OpenGLOffscreenBuffer::unbind()
        {
            if (!_id)
                return;
            //DJV_DEBUG("OpenGLOffscreenBuffer::unbind");
            //DJV_DEBUG_PRINT("id = " << static_cast<int>(_id));
            //DJV_DEBUG_PRINT("restore = " << static_cast<int>(_restore));
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            DJV_DEBUG_OPEN_GL(glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _restore));
            _restore = 0;
        }

        const QStringList & OpenGLOffscreenBuffer::errorLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::OpenGLOffscreenBuffer", "Cannot create texture") <<
                qApp->translate("djv::Graphics::OpenGLOffscreenBuffer", "Cannot initialize texture: %1") <<
                qApp->translate("djv::Graphics::OpenGLOffscreenBuffer", "Cannot create FBO") <<
                qApp->translate("djv::Graphics::OpenGLOffscreenBuffer", "Cannot initialize FBO: %1");
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
