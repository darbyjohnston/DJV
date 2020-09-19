// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvGL/ShaderSystem.h>

#include <djvGL/Shader.h>

#include <djvSystem/Context.h>
#include <djvSystem/CoreSystem.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/FileIOFunc.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/ResourceSystem.h>

#include <map>

using namespace djv::Core;

namespace djv
{
    namespace GL
    {
        struct ShaderSystem::Private
        {
            std::map<std::pair<std::string, std::string>, std::shared_ptr<Shader> > shaders;
        };

        void ShaderSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::GL::Render::ShaderSystem", context);
            
            addDependency(context->getSystemT<System::CoreSystem>());
        }

        ShaderSystem::ShaderSystem() :
            _p(new Private)
        {}

        ShaderSystem::~ShaderSystem()
        {}

        std::shared_ptr<ShaderSystem> ShaderSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ShaderSystem>(new ShaderSystem);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Shader> ShaderSystem::getShader(
            const std::string& vertexSource,
            const std::string& fragmentSource)
        {
            DJV_PRIVATE_PTR();
            std::shared_ptr<Shader> out;
            const auto key = std::make_pair(vertexSource, fragmentSource);
            const auto i = p.shaders.find(key);
            if (i != p.shaders.end())
            {
                out = i->second;
            }
            else
            {
                if (auto context = getContext().lock())
                {
                    auto resourceSystem = context->getSystemT<System::ResourceSystem>();
                    const System::File::Path shaderPath = resourceSystem->getPath(System::File::ResourcePath::Shaders);

                    auto io = System::File::IO::create();
                    std::string vertexFileName = std::string(System::File::Path(shaderPath, vertexSource));
                    io->open(vertexFileName, System::File::IO::Mode::Read);
                    std::string vertexSource = System::File::readContents(io);

                    std::string fragmentFileName = std::string(System::File::Path(shaderPath, fragmentSource));
                    io->open(fragmentFileName, System::File::IO::Mode::Read);
                    std::string fragmentSource = System::File::readContents(io);

                    out = Shader::create(vertexSource, fragmentSource);
                    out->setVertexName(vertexFileName);
                    out->setFragmentName(fragmentFileName);
                    p.shaders[key] = out;
                }
            }
            return out;
        }
            
    } // namespace GL
} // namespace djv

