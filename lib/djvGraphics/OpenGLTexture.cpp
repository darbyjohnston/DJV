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

#include <djvGraphics/OpenGLTexture.h>

#include <djvGraphics/OpenGLImage.h>

#include <djvCore/Debug.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        OpenGLTexture::~OpenGLTexture()
        {
            del();
        }

        void OpenGLTexture::init(
            const PixelDataInfo & info,
            GLenum                target,
            GLenum                min,
            GLenum                mag) throw (djvError)
        {
            if (info == _info && target == _target && min == _min && mag == _mag)
                return;

            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

            del();

            //DJV_DEBUG("OpenGLTexture::init");
            //DJV_DEBUG_PRINT("info = " << info);

            _info = info;
            _target = target;
            _min = min;
            _mag = mag;
            DJV_DEBUG_OPEN_GL(glGenTextures(1, &_id));
            //DJV_DEBUG_PRINT("id = " << int(_id));
            if (!_id)
            {
                throw djvError(
                    "djv::Graphics::OpenGLTexture",
                    qApp->translate("djv::Graphics::OpenGLTexture", "Cannot create texture"));
            }

            DJV_DEBUG_OPEN_GL(glFuncs->glBindTexture(_target, _id));
            DJV_DEBUG_OPEN_GL(glFuncs->glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            DJV_DEBUG_OPEN_GL(glFuncs->glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            DJV_DEBUG_OPEN_GL(glFuncs->glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, _min));
            DJV_DEBUG_OPEN_GL(glFuncs->glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, _mag));

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
                _target,
                0,
                format,
                _info.size.x,
                _info.size.y,
                0,
                OpenGLUtil::format(_info.pixel, _info.bgr),
                OpenGLUtil::type(_info.pixel),
                0);
            GLenum error = glFuncs->glGetError();
            if (error != GL_NO_ERROR)
            {
                throw djvError(
                    "djv::Graphics::OpenGLTexture",
                    qApp->translate("djv::Graphics::OpenGLTexture", "Cannot create texture: %1").
                    arg(OpenGLUtil::errorString(error)));
            }
        }

        void OpenGLTexture::init(
            const PixelData & data,
            GLenum            target,
            GLenum            min,
            GLenum            mag) throw (djvError)
        {
            init(data.info(), target, min, mag);
            copy(data);
        }

        void OpenGLTexture::copy(const PixelData & in)
        {
            //DJV_DEBUG("OpenGLTexture::copy");
            //DJV_DEBUG_PRINT("in = " << in);
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            bind();
            const PixelDataInfo & info = in.info();
            OpenGLImage::stateUnpack(in.info());
            GLenum format = OpenGLUtil::format(info.pixel, info.bgr);
            GLenum type = OpenGLUtil::type(info.pixel);
            //DJV_DEBUG_PRINT("target = " << _target);
            //DJV_DEBUG_PRINT("format = " << format);
            //DJV_DEBUG_PRINT("type = " << type);
            DJV_DEBUG_OPEN_GL(
                glFuncs->glTexSubImage2D(
                    _target,
                    0,
                    0,
                    0,
                    info.size.x,
                    info.size.y,
                    format,
                    type,
                    in.data()));
        }

        void OpenGLTexture::copy(const PixelData & in, const djvBox2i & area)
        {
            //DJV_DEBUG("OpenGLTexture::copy");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("area = " << area);
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
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
            DJV_DEBUG_OPEN_GL(glFuncs->glTexSubImage2D(
                _target,
                0,
                0,
                0,
                area.size.x,
                area.size.y,
                OpenGLUtil::format(info.pixel, info.bgr),
                OpenGLUtil::type(info.pixel),
                in.data()));
        }

        void OpenGLTexture::copy(const glm::ivec2 & in)
        {
            //DJV_DEBUG("OpenGLTexture::copy");
            //DJV_DEBUG_PRINT("in = " << in);
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            DJV_DEBUG_OPEN_GL(glFuncs->glBindTexture(_target, _id));
            DJV_DEBUG_OPEN_GL(
                glFuncs->glCopyTexSubImage2D(
                    _target,
                    0,
                    0,
                    0,
                    0,
                    0,
                    in.x,
                    in.y));
        }

        void OpenGLTexture::bind()
        {
            //DJV_DEBUG("OpenGLTexture::bind");
            auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
            DJV_DEBUG_OPEN_GL(glFuncs->glBindTexture(_target, _id));
        }

        const PixelDataInfo & OpenGLTexture::info() const
        {
            return _info;
        }

        GLenum OpenGLTexture::target() const
        {
            return _target;
        }

        GLenum OpenGLTexture::min() const
        {
            return _min;
        }

        GLenum OpenGLTexture::mag() const
        {
            return _mag;
        }

        GLuint OpenGLTexture::id() const
        {
            return _id;
        }

        void OpenGLTexture::del()
        {
            if (_id)
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
                glFuncs->glDeleteTextures(1, &_id);
                _id = 0;
            }
        }

    } // namespace Graphics
} // namespace djv
