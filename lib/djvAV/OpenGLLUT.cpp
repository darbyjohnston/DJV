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

#include <djvGraphics/OpenGLLUT.h>

#include <djvGraphics/OpenGLImage.h>

#include <djvCore/Debug.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        struct OpenGLLUT::Private
        {
            PixelDataInfo info;
            GLuint        id = 0;
        };

        OpenGLLUT::OpenGLLUT() :
            _p(new Private)
        {}

        OpenGLLUT::~OpenGLLUT()
        {
            del();
        }

        void OpenGLLUT::init(const PixelDataInfo & info)
        {
            if (info == _p->info)
                return;

            //DJV_DEBUG("OpenGLLUT::init");
            //DJV_DEBUG_PRINT("info = " << _p->info);

            del();

            _p->info = info;

            glGenTextures(1, &_p->id);
            if (!_p->id)
            {
                throw Core::Error(
                    "djv::Graphics::OpenGLLUT",
                    qApp->translate("djv::Graphics::OpenGLLUT", "Cannot create texture"));
            }

            glBindTexture(GL_TEXTURE_2D, _p->id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                OpenGL::internalFormat(_p->info.pixel),
                _p->info.size.x,
                _p->info.size.y,
                0,
                OpenGL::format(_p->info.pixel, _p->info.bgr),
                OpenGL::type(_p->info.pixel),
                0);
        }

        void OpenGLLUT::init(const PixelData & data)
        {
            init(data.info());
            bind();
            copy(data);
        }

        const PixelDataInfo & OpenGLLUT::info() const
        {
            return _p->info;
        }

        GLuint OpenGLLUT::id() const
        {
            return _p->id;
        }

        void OpenGLLUT::bind()
        {
            //DJV_DEBUG("OpenGLLUT::bind");
            glBindTexture(GL_TEXTURE_2D, _p->id);
        }

        void OpenGLLUT::copy(const PixelData & in)
        {
            //DJV_DEBUG("OpenGLLUT::copy");
            //DJV_DEBUG_PRINT("in = " << in);
            const PixelDataInfo & info = in.info();
            OpenGLImage::stateUnpack(in.info());
            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0,
                0,
                info.size.x,
                info.size.y,
                OpenGL::format(info.pixel, info.bgr),
                OpenGL::type(info.pixel),
                in.data());
        }

        void OpenGLLUT::del()
        {
            if (_p->id)
            {
                glDeleteTextures(1, &_p->id);
                _p->id = 0;
            }
        }

    } // namespace Graphics
} // namespace djv
