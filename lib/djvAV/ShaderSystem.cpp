// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

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
                const auto key = std::make_pair(vertex, fragment);
                const auto i = p.shaders.find(key);
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

                        auto io = FileSystem::FileIO::create();
                        std::string vertexFileName = std::string(FileSystem::Path(shaderPath, vertex));
                        io->open(vertexFileName, FileSystem::FileIO::Mode::Read);
                        std::string vertexSource = FileSystem::FileIO::readContents(io);

                        std::string fragmentFileName = std::string(FileSystem::Path(shaderPath, fragment));
                        io->open(fragmentFileName, FileSystem::FileIO::Mode::Read);
                        std::string fragmentSource = FileSystem::FileIO::readContents(io);

                        auto shader = AV::Render::Shader::create(vertexSource, fragmentSource);
                        shader->setVertexName(vertexFileName);
                        shader->setFragmentName(fragmentFileName);
                        out = OpenGL::Shader::create(shader);

                        p.shaders[key] = out;
                    }
                }
                return out;
            }
            
        } // namespace Render
    } // namespace AV
} // namespace djv

