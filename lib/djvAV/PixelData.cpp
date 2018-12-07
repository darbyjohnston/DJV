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

#include <djvAV/PixelData.h>

#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/OpenGLTexture.h>
#include <djvAV/Shader.h>
#include <djvAV/Shape.h>
#include <djvAV/System.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Context.h>

#include <glm/gtc/matrix_transform.hpp>

namespace djv
{
    namespace AV
    {
        namespace Pixel
        {
            void Mirror::setX(bool value)
            {
                _x = value;
            }

            void Mirror::setY(bool value)
            {
                _y = value;
            }

            void Layout::setMirror(const Mirror & value)
            {
                _mirror = value;
            }

            void Layout::setAlignment(GLint value)
            {
                _alignment = value;
            }

            void Layout::setEndian(Core::Memory::Endian value)
            {
                _endian = value;
            }

            void Info::setName(const std::string & name)
            {
                _name = name;
            }

            void Data::_init(const Info & info)
            {
                _info = info;
                _pixelByteCount = info.getPixelByteCount();
                _scanlineByteCount = info.getScanlineByteCount();
                _dataByteCount = info.getDataByteCount();
                if (_dataByteCount)
                {
                    _data = new uint8_t[_dataByteCount];
                    _p = _data;
                }
            }

            Data::Data()
            {}

            Data::~Data()
            {
                delete[] _data;
            }

            std::shared_ptr<Data> Data::create(const Info& info)
            {
                auto out = std::shared_ptr<Data>(new Data);
                out->_init(info);
                return out;
            }

            uint8_t * Data::getData()
            {
                return _data;
            }

            uint8_t* Data::getData(int y)
            {
                return _data + y * _scanlineByteCount;
            }

            uint8_t * Data::getData(int x, int y)
            {
                return _data + y * _scanlineByteCount + x * _pixelByteCount;
            }

            void Data::zero()
            {
                memset(_data, 0, _dataByteCount);
            }

            std::shared_ptr<Data> Data::convert(Type type, const std::shared_ptr<Core::Context> & context) const
            {
                const auto size = _info.getSize();
                const auto info = Info(size, type);
                auto out = Data::create(info);

                auto offscreenBuffer = OpenGL::OffscreenBuffer::create(info);
                const OpenGL::OffscreenBufferBinding binding(offscreenBuffer);
                        
                auto texture = AV::OpenGL::Texture::create(info);
                texture->copy(*this);
                texture->bind();

                AV::Shape::Square square;
                AV::TriangleMesh mesh;
                square.triangulate(mesh);
                auto vbo = AV::OpenGL::VBO::create(2, 3, AV::OpenGL::VBOType::Pos3_F32_UV_U16_Normal_U10);
                vbo->copy(AV::OpenGL::VBO::convert(mesh, vbo->getType()));
                auto vao = AV::OpenGL::VAO::create(vbo->getType(),vbo->getID());
                vao->bind();

                auto shader = AV::OpenGL::Shader::create(AV::Shader::create(
                    context->getResourcePath(Core::ResourcePath::ShadersDirectory, "djvAVPixelDataConvertVertex.glsl"),
                    context->getResourcePath(Core::ResourcePath::ShadersDirectory, "djvAVPixelDataConvertFragment.glsl")));
                shader->bind();
                shader->setUniform("textureSampler", 0);
                glm::mat4x4 modelMatrix(1);
                modelMatrix = glm::rotate(modelMatrix, Core::Math::deg2rad(-90.f), glm::vec3(1.f, 0.f, 0.f));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(size.x, 0.f, size.y));
                modelMatrix = glm::translate(modelMatrix, glm::vec3(.5f, 0.f, .5f));
                glm::mat4x4 viewMatrix(1);
                glm::mat4x4 projectionMatrix(1);
                projectionMatrix = glm::ortho(
                    0.f,
                    static_cast<float>(size.x),
                    0.f,
                    static_cast<float>(size.y),
                    -1.f,
                    1.f);
                shader->setUniform("transform.mvp", projectionMatrix * viewMatrix * modelMatrix);

                auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                glFuncs->glViewport(0, 0, size.x, size.y);
                glFuncs->glClearColor(0.f, 0.f, 0.f, 0.f);
                glFuncs->glClear(GL_COLOR_BUFFER_BIT);
                glFuncs->glActiveTexture(GL_TEXTURE0);
                        
                vao->draw(0, 6);

                glFuncs->glPixelStorei(GL_PACK_ALIGNMENT, 1);
                glFuncs->glReadPixels(
                    0, 0, size.x, size.y,
                    info.getGLFormat(),
                    info.getGLType(),
                    out->getData());

                return out;
            }

            bool Data::operator == (const Data & other) const
            {
                if (other._info == _info)
                {
                    if (GL_UNSIGNED_INT_10_10_10_2 == _info.getGLType())
                    {
                        const auto & size = _info.getSize();
                        for (int y = 0; y < size.y; ++y)
                        {
                            const U10_S * p = reinterpret_cast<const U10_S*>(getData(y));
                            const U10_S * otherP = reinterpret_cast<const U10_S*>(other.getData(y));
                            for (int x = 0; x < size.x; ++x, ++p, ++otherP)
                            {
                                if (*p != *otherP)
                                {
                                    return false;
                                }
                            }
                        }
                        return true;
                    }
                    else
                    {
                        return 0 == memcmp(other._p, _p, _dataByteCount);
                    }
                }
                return false;
            }

        } // namespace Pixel
    } // namespace AV
} // namespace djv

