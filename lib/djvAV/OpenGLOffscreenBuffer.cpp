//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
            namespace
            {
                //! \todo Should this be configurable?
                const size_t samples = 4;

                enum class Error
                {
                    Texture,
                    Create,
                    Init
                };
                
                std::string getErrorMessage(Error error)
                {
                    std::stringstream ss;
                    switch (error)
                    {
                    case Error::Texture:
                        ss << DJV_TEXT("The OpenGL texture cannot be created.");
                        break;
                    case Error::Create:
                        ss << DJV_TEXT("The OpenGL frame buffer cannot be created.");
                        break;
                    case Error::Init:
                        ss << DJV_TEXT("The OpenGL frame buffer cannot be initialized.");
                        break;
                    default: break;
                    }
                    return ss.str();
                }
                
            } // namespace

            OffscreenBufferError::OffscreenBufferError(const std::string& what) :
                std::runtime_error(what)
            {}
            
            void OffscreenBuffer::_init(const Image::Info & info, OffscreenType type)
            {
                _info = info;
                _type = type;

                // Create the texture.
                //! \bug Fall back to a regular offscreen buffer if multi-sampling is not available.
                glGenTextures(1, &_textureID);
                if (!_textureID)
                {
                    throw OffscreenBufferError(getErrorMessage(Error::Texture));
                }
                GLenum target = GL_TEXTURE_2D;
#if defined(DJV_OPENGL_ES2)
#else // DJV_OPENGL_ES2
                switch (type)
                {
                case OffscreenType::MultiSample: target = GL_TEXTURE_2D_MULTISAMPLE; break;
                default: break;
                }
#endif // DJV_OPENGL_ES2
                glBindTexture(target, _textureID);
                glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#if defined(DJV_OPENGL_ES2)
                glTexImage2D(
                    target,
                    0,
                    Texture::getInternalFormat(_info.type),
                    _info.size.w,
                    _info.size.h,
                    0,
                    _info.getGLFormat(),
                    _info.getGLType(),
                    0);
#else // DJV_OPENGL_ES2
                switch (type)
                {
                case OffscreenType::MultiSample:
                    glTexImage2DMultisample(
                        target,
                        static_cast<GLsizei>(samples),
                        Texture::getInternalFormat(_info.type),
                        _info.size.w,
                        _info.size.h,
                        false);
                    break;
                default:
                    glTexImage2D(
                        target,
                        0,
                        Texture::getInternalFormat(_info.type),
                        _info.size.w,
                        _info.size.h,
                        0,
                        _info.getGLFormat(),
                        _info.getGLType(),
                        0);
                    break;
                }
#endif // DJV_OPENGL_ES2
                glBindTexture(target, 0);

                // Create the FBO.
                glGenFramebuffers(1, &_id);
                if (!_id)
                {
                    throw OffscreenBufferError(getErrorMessage(Error::Create));
                }
                const OffscreenBufferBinding binding(shared_from_this());
                glFramebufferTexture2D(
                    GL_FRAMEBUFFER,
                    GL_COLOR_ATTACHMENT0,
                    target,
                    _textureID,
                    0);
                GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (error != GL_FRAMEBUFFER_COMPLETE)
                {
                    throw OffscreenBufferError(getErrorMessage(Error::Init));
                }
            }

            OffscreenBuffer::~OffscreenBuffer()
            {
                if (_id)
                {
                    glDeleteFramebuffers(1, &_id);
                    _id = 0;
                }
                if (_textureID)
                {
                    glDeleteTextures(1, &_textureID);
                    _textureID = 0;
                }
            }

            std::shared_ptr<OffscreenBuffer> OffscreenBuffer::create(const Image::Info & info, OffscreenType type)
            {
                auto out = std::shared_ptr<OffscreenBuffer>(new OffscreenBuffer);
                out->_init(info, type);
                return out;
            }

            void OffscreenBuffer::bind()
            {
                glBindFramebuffer(GL_FRAMEBUFFER, _id);
            }

            OffscreenBufferBinding::OffscreenBufferBinding(const std::shared_ptr<OffscreenBuffer> & buffer) :
                _buffer(buffer)
            {
                _buffer->bind();
            }

            OffscreenBufferBinding::~OffscreenBufferBinding()
            {}

        } // namespace OpenGL
    } // namespace AV
} // namespace djv
