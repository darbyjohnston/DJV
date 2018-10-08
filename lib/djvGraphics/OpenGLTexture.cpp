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

#include <djvGraphics/OpenGLTexture.h>

#include <djvGraphics/OpenGLImage.h>

#include <djvCore/Debug.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        struct OpenGLTexture::Private
        {
            PixelDataInfo info;
            GLenum        target = GL_NONE;
            GLenum        min = GL_NONE;
            GLenum        mag = GL_NONE;
            GLuint        id = 0;
        };

        OpenGLTexture::OpenGLTexture() :
            _p(new Private)
        {}

        OpenGLTexture::~OpenGLTexture()
        {
            del();
        }

        void OpenGLTexture::init(
            const PixelDataInfo & info,
            GLenum                target,
            GLenum                min,
            GLenum                mag)
        {
            if (info == _p->info && target == _p->target && min == _p->min && mag == _p->mag)
                return;

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

            del();

            //DJV_DEBUG("OpenGLTexture::init");
            //DJV_DEBUG_PRINT("info = " << info);

            _p->info = info;
            _p->target = target;
            _p->min = min;
            _p->mag = mag;
            glGenTextures(1, &_p->id);
            //DJV_DEBUG_PRINT("id = " << int(_p->id));
            if (!_p->id)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLTexture",
                    qApp->translate("djv::Graphics::OpenGLTexture", "Cannot create texture"));
            }

            glFuncs->glBindTexture(_p->target, _p->id);
            glFuncs->glTexParameteri(_p->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glFuncs->glTexParameteri(_p->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFuncs->glTexParameteri(_p->target, GL_TEXTURE_MIN_FILTER, _p->min);
            glFuncs->glTexParameteri(_p->target, GL_TEXTURE_MAG_FILTER, _p->mag);
            glFuncs->glTexImage2D(
                _p->target,
                0,
                OpenGL::internalFormat(_p->info.pixel),
                _p->info.size.x,
                _p->info.size.y,
                0,
                OpenGL::format(_p->info.pixel, _p->info.bgr),
                OpenGL::type(_p->info.pixel),
                0);
        }

        void OpenGLTexture::init(
            const PixelData & data,
            GLenum            target,
            GLenum            min,
            GLenum            mag)
        {
            init(data.info(), target, min, mag);
            copy(data);
        }

        void OpenGLTexture::copy(const PixelData & in)
        {
            //DJV_DEBUG("OpenGLTexture::copy");
            //DJV_DEBUG_PRINT("in = " << in);
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            bind();
            const PixelDataInfo & info = in.info();
            OpenGLImage::stateUnpack(in.info());
            GLenum format = OpenGL::format(info.pixel, info.bgr);
            GLenum type = OpenGL::type(info.pixel);
            //DJV_DEBUG_PRINT("target = " << _target);
            //DJV_DEBUG_PRINT("format = " << format);
            //DJV_DEBUG_PRINT("type = " << type);
            glFuncs->glTexSubImage2D(
                _p->target,
                0,
                0,
                0,
                info.size.x,
                info.size.y,
                format,
                type,
                in.data());
        }

        void OpenGLTexture::copy(const PixelData & in, const Core::Box2i & area)
        {
            //DJV_DEBUG("OpenGLTexture::copy");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("area = " << area);
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            bind();
            const PixelDataInfo & info = in.info();
            glm::ivec2 position = area.position;
            if (info.mirror.x)
            {
                position.x = info.size.x - area.position.x - area.size.x;
            }
            if (info.mirror.y)
            {
                position.y = info.size.y - area.position.y - area.size.y;
            }
            OpenGLImage::stateUnpack(in.info(), position);
            glFuncs->glTexSubImage2D(
                _p->target,
                0,
                0,
                0,
                area.size.x,
                area.size.y,
                OpenGL::format(info.pixel, info.bgr),
                OpenGL::type(info.pixel),
                in.data());
        }

        void OpenGLTexture::copy(const glm::ivec2 & in)
        {
            //DJV_DEBUG("OpenGLTexture::copy");
            //DJV_DEBUG_PRINT("in = " << in);
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glBindTexture(_p->target, _p->id);
            glFuncs->glCopyTexSubImage2D(
                _p->target,
                0,
                0,
                0,
                0,
                0,
                in.x,
                in.y);
        }

        void OpenGLTexture::bind()
        {
            //DJV_DEBUG("OpenGLTexture::bind");
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
            glFuncs->glBindTexture(_p->target, _p->id);
        }

        const PixelDataInfo & OpenGLTexture::info() const
        {
            return _p->info;
        }

        GLenum OpenGLTexture::target() const
        {
            return _p->target;
        }

        GLenum OpenGLTexture::min() const
        {
            return _p->min;
        }

        GLenum OpenGLTexture::mag() const
        {
            return _p->mag;
        }

        GLuint OpenGLTexture::id() const
        {
            return _p->id;
        }

        void OpenGLTexture::del()
        {
            if (_p->id)
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glDeleteTextures(1, &_p->id);
                _p->id = 0;
            }
        }

    } // namespace Graphics
} // namespace djv
