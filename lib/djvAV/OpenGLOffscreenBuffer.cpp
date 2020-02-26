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
                enum class Error
                {
                    ColorTexture,
                    DepthTexture,
                    Create,
                    Init
                };
                
                std::string getErrorMessage(Error error)
                {
                    //! \todo How can we translate these?
                    std::stringstream ss;
                    switch (error)
                    {
                    case Error::ColorTexture:
                        ss << DJV_TEXT("error_opengl_color_texture_creation");
                        break;
                    case Error::DepthTexture:
                        ss << DJV_TEXT("error_opengl_depth_texture_creation");
                        break;
                    case Error::Create:
                        ss << DJV_TEXT("error_opengl_frame_buffer_creation");
                        break;
                    case Error::Init:
                        ss << DJV_TEXT("error_opengl_frame_buffer_init");
                        break;
                    default: break;
                    }
                    return ss.str();
                }
                
            } // namespace

            GLenum getInternalFormat(OffscreenDepthType value)
            {
                const std::vector<GLenum> data =
                {
                    GL_NONE,
                    GL_DEPTH_COMPONENT24,
                    GL_DEPTH_COMPONENT32F
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(OffscreenDepthType::Count));
                return data[static_cast<size_t>(value)];
            }

            GLenum getGLFormat(OffscreenDepthType value)
            {
                const std::vector<GLenum> data =
                {
                    GL_NONE,
                    GL_DEPTH_COMPONENT,
                    GL_DEPTH_COMPONENT
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(OffscreenDepthType::Count));
                return data[static_cast<size_t>(value)];
            }

            GLenum getGLType(OffscreenDepthType value)
            {
                const std::vector<GLenum> data =
                {
                    GL_NONE,
                    GL_UNSIGNED_INT,
                    GL_FLOAT
                };
                DJV_ASSERT(data.size() == static_cast<size_t>(OffscreenDepthType::Count));
                return data[static_cast<size_t>(value)];
            }

            OffscreenBufferError::OffscreenBufferError(const std::string& what) :
                std::runtime_error(what)
            {}
            
            void OffscreenBuffer::_init(
                const Image::Size& size,
                Image::Type colorType,
                OffscreenDepthType depthType,
                OffscreenSampling sampling)
            {
                _size = size;
                _colorType = colorType;
                _depthType = depthType;
                _sampling = sampling;

                GLenum target = GL_TEXTURE_2D;
                size_t samples = 0;
#if !defined(DJV_OPENGL_ES2)
                switch (sampling)
                {
                case OffscreenSampling::_2:
                    samples = 2;
                    target = GL_TEXTURE_2D_MULTISAMPLE;
                    break;
                case OffscreenSampling::_4:
                    samples = 4;
                    target = GL_TEXTURE_2D_MULTISAMPLE;
                    break;
                case OffscreenSampling::_8:
                    samples = 8;
                    target = GL_TEXTURE_2D_MULTISAMPLE;
                    break;
                case OffscreenSampling::_16:
                    samples = 16;
                    target = GL_TEXTURE_2D_MULTISAMPLE;
                    break;
                default: break;
                }
#endif // DJV_OPENGL_ES2

                if (colorType != Image::Type::None)
                {
                    // Create the color texture.
                    //! \bug Fall back to a regular offscreen buffer if multi-sampling is not available.
                    glGenTextures(1, &_colorID);
                    if (!_colorID)
                    {
                        throw OffscreenBufferError(getErrorMessage(Error::ColorTexture));
                    }
                    glBindTexture(target, _colorID);
#if defined(DJV_OPENGL_ES2)
                    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexImage2D(
                        target,
                        0,
                        Texture::getInternalFormat(colorType),
                        size.w,
                        size.h,
                        0,
                        Image::getGLFormat(colorType),
                        Image::getGLType(colorType),
                        0);
#else // DJV_OPENGL_ES2
                    switch (sampling)
                    {
                    case OffscreenSampling::_2:
                    case OffscreenSampling::_4:
                    case OffscreenSampling::_8:
                    case OffscreenSampling::_16:
                        glTexImage2DMultisample(
                            target,
                            static_cast<GLsizei>(samples),
                            Texture::getInternalFormat(colorType),
                            size.w,
                            size.h,
                            false);
                        break;
                    default:
                        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        glTexImage2D(
                            target,
                            0,
                            Texture::getInternalFormat(colorType),
                            size.w,
                            size.h,
                            0,
                            Image::getGLFormat(colorType),
                            Image::getGLType(colorType),
                            0);
                        break;
                    }
#endif // DJV_OPENGL_ES2
                }

                if (depthType != OffscreenDepthType::None)
                {
                    // Create the depth texture.
                    //! \bug Fall back to a regular offscreen buffer if multi-sampling is not available.
                    glGenTextures(1, &_depthID);
                    if (!_depthID)
                    {
                        throw OffscreenBufferError(getErrorMessage(Error::DepthTexture));
                    }
                    glBindTexture(target, _depthID);
#if defined(DJV_OPENGL_ES2)
                    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexImage2D(
                        target,
                        0,
                        getInternalFormat(depthType),
                        size.w,
                        size.h,
                        0,
                        getGLFormat(depthType),
                        getGLType(depthType),
                        0);
#else // DJV_OPENGL_ES2
                    switch (sampling)
                    {
                    case OffscreenSampling::_2:
                    case OffscreenSampling::_4:
                    case OffscreenSampling::_8:
                    case OffscreenSampling::_16:
                        glTexImage2DMultisample(
                            target,
                            static_cast<GLsizei>(samples),
                            getInternalFormat(depthType),
                            size.w,
                            size.h,
                            false);
                        break;
                    default:
                        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        glTexImage2D(
                            target,
                            0,
                            getInternalFormat(depthType),
                            size.w,
                            size.h,
                            0,
                            getGLFormat(depthType),
                            getGLType(depthType),
                            0);
                        break;
                    }
#endif // DJV_OPENGL_ES2
                }

                // Create the FBO.
                glGenFramebuffers(1, &_id);
                if (!_id)
                {
                    throw OffscreenBufferError(getErrorMessage(Error::Create));
                }
                const OffscreenBufferBinding binding(shared_from_this());
                if (colorType != Image::Type::None)
                {
                    glFramebufferTexture2D(
                        GL_FRAMEBUFFER,
                        GL_COLOR_ATTACHMENT0,
                        target,
                        _colorID,
                        0);
                }
                if (depthType != OffscreenDepthType::None)
                {
                    glFramebufferTexture2D(
                        GL_FRAMEBUFFER,
                        GL_DEPTH_ATTACHMENT,
                        target,
                        _depthID,
                        0);
                }
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
                if (_colorID)
                {
                    glDeleteTextures(1, &_colorID);
                    _colorID = 0;
                }
                if (_depthID)
                {
                    glDeleteTextures(1, &_depthID);
                    _depthID = 0;
                }
            }

            std::shared_ptr<OffscreenBuffer> OffscreenBuffer::create(
                const Image::Size& size,
                Image::Type colorType,
                OffscreenDepthType depthType,
                OffscreenSampling sampling)
            {
                auto out = std::shared_ptr<OffscreenBuffer>(new OffscreenBuffer);
                out->_init(size, colorType, depthType, sampling);
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

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::OpenGL,
        OffscreenDepthType,
        DJV_TEXT("offscreen_depth_type_none"),
        DJV_TEXT("offscreen_depth_type_24"),
        DJV_TEXT("offscreen_depth_type_32"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::OpenGL,
        OffscreenSampling,
        DJV_TEXT("offscreen_sampling_none"),
        DJV_TEXT("offscreen_sampling_2"),
        DJV_TEXT("offscreen_sampling_4"),
        DJV_TEXT("offscreen_sampling_8"),
        DJV_TEXT("offscreen_sampling_16"));

} // namespace djv
