//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/OpenGLOffscreenBuffer.h>

#include <djvAV/OpenGLTexture.h>

#include <djvCore/Error.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            struct OffscreenBuffer::Private
            {
                Pixel::Info info;
                GLuint id = 0;
                GLuint textureID = 0;
                GLint restore = 0;
            };

            void OffscreenBuffer::_init(const Pixel::Info& info)
            {
                _p->info = info;

                // Create the texture.
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glGenTextures(1, &_p->textureID);
                if (!_p->textureID)
                {
                    throw Core::Error(
                        "djv::AV::OpenGL::OffscreenBuffer",
                        DJV_TEXT("Cannot create texture"));
                }
                glFuncs->glBindTexture(GL_TEXTURE_2D, _p->textureID);
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glFuncs->glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    Texture::getInternalFormat(_p->info.getType()),
                    _p->info.getWidth(),
                    _p->info.getHeight(),
                    0,
                    _p->info.getGLFormat(),
                    _p->info.getGLType(),
                    0);
                glFuncs->glBindTexture(GL_TEXTURE_2D, 0);

                // Create the FBO.
                glFuncs->glGenFramebuffers(1, &_p->id);
                if (!_p->id)
                {
                    throw Core::Error(
                        "djv::AV::OpenGL::OffscreenBuffer",
                        DJV_TEXT("Cannot create frame buffer"));
                }
                const OffscreenBufferBinding binding(shared_from_this());
                glFuncs->glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_2D,
                    _p->textureID,
                    0);
                GLenum error = glFuncs->glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (error != GL_FRAMEBUFFER_COMPLETE)
                {
                    throw Core::Error(
                        "djv::AV::OpenGL::OffscreenBuffer",
                        DJV_TEXT("Cannot iniitialize frame buffer"));
                }
            }

            OffscreenBuffer::OffscreenBuffer() :
                _p(new Private)
            {}

            OffscreenBuffer::~OffscreenBuffer()
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                if (_p->id)
                {
                    glFuncs->glDeleteFramebuffers(1, &_p->id);
                    _p->id = 0;
                }
                if (_p->textureID)
                {
                    glFuncs->glDeleteTextures(1, &_p->textureID);
                    _p->textureID = 0;
                }
            }

            std::shared_ptr<OffscreenBuffer> OffscreenBuffer::create(const Pixel::Info& info)
            {
                auto out = std::shared_ptr<OffscreenBuffer>(new OffscreenBuffer);
                out->_init(info);
                return out;
            }

            const Pixel::Info & OffscreenBuffer::getInfo() const
            {
                return _p->info;
            }

            GLuint OffscreenBuffer::getID() const
            {
                return _p->id;
            }

            GLuint OffscreenBuffer::getTextureID() const
            {
                return _p->textureID;
            }

            void OffscreenBuffer::bind()
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_p->restore);
                glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _p->id);
            }

            void OffscreenBuffer::unbind()
            {
                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, _p->restore);
                _p->restore = 0;
            }

            OffscreenBufferBinding::OffscreenBufferBinding(const std::shared_ptr<OffscreenBuffer> & buffer) :
                _buffer(buffer)
            {
                _buffer->bind();
            }

            OffscreenBufferBinding::~OffscreenBufferBinding()
            {
                _buffer->unbind();
            }

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
