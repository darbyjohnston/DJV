//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvAV/ShaderSystem.h>

#include <djvAV/OpenGLShader.h>
#include <djvAV/Shader.h>

#include <djvCore/Context.h>
#include <djvCore/FileIO.h>
#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>

#include <map>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Render
        {
            struct ShaderSystem::Private
            {
                std::map<std::pair<std::string, std::string>, std::shared_ptr<OpenGL::Shader> > shaders;
            };

            void ShaderSystem::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::AV::Render::ShaderSystem", context);
            }

            ShaderSystem::ShaderSystem() :
                _p(new Private)
            {}

            ShaderSystem::~ShaderSystem()
            {}

            std::shared_ptr<ShaderSystem> ShaderSystem::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ShaderSystem>(new ShaderSystem);
                out->_init(context);
                return out;
            }

            std::shared_ptr<OpenGL::Shader> ShaderSystem::getShader(const std::string& vertex, const std::string& fragment)
            {
                DJV_PRIVATE_PTR();
                std::shared_ptr<OpenGL::Shader> out;
                const auto i = p.shaders.find(std::make_pair(vertex, fragment));
                if (i != p.shaders.end())
                {
                    out = i->second;
                }
                else
                {
                    if (auto context = getContext().lock())
                    {
                        auto resourceSystem = context->getSystemT<ResourceSystem>();
                        const FileSystem::Path shaderPath = resourceSystem->getPath(FileSystem::ResourcePath::Shaders);

                        FileSystem::FileIO io;
                        std::string vertexFileName = std::string(FileSystem::Path(shaderPath, vertex));
                        io.open(vertexFileName, FileSystem::FileIO::Mode::Read);
                        std::string vertexSource = FileSystem::FileIO::readContents(io);

                        std::string fragmentFileName = std::string(FileSystem::Path(shaderPath, fragment));
                        io.open(fragmentFileName, FileSystem::FileIO::Mode::Read);
                        std::string fragmentSource = FileSystem::FileIO::readContents(io);

                        auto shader = AV::Render::Shader::create(vertexSource, fragmentSource);
                        shader->setVertexName(vertexFileName);
                        shader->setFragmentName(fragmentFileName);
                        out = OpenGL::Shader::create(shader);
                    }
                }
                return out;
            }
            
        } // namespace Render
    } // namespace AV
} // namespace djv

