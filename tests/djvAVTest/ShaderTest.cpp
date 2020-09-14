// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/ShaderTest.h>

#include <djvAV/Shader.h>

#include <djvCore/Context.h>
#include <djvCore/ErrorFunc.h>
#include <djvCore/ResourceSystem.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        ShaderTest::ShaderTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::ShaderTest", tempPath, context)
        {}
        
        void ShaderTest::run()
        {
            if (auto context = getContext().lock())
            {
                {
                    const std::string vertex = "vertex";
                    const std::string fragment = "fragment";
                    auto shader = Render::Shader::create(vertex, fragment);
                    DJV_ASSERT(vertex == shader->getVertexSource());
                    DJV_ASSERT(fragment == shader->getFragmentSource());
                }

                {
                    auto resourceSystem = context->getSystemT<ResourceSystem>();
                    const FileSystem::Path shaderPath = resourceSystem->getPath(FileSystem::ResourcePath::Shaders);
                    const FileSystem::Path vertexPath = FileSystem::Path(shaderPath, "djvAVImageConvertVertex.glsl");
                    const FileSystem::Path fragmentPath = FileSystem::Path(shaderPath, "djvAVImageConvertFragment.glsl");
                    auto shader = Render::Shader::create(vertexPath, fragmentPath);
                    DJV_ASSERT(vertexPath.get() == shader->getVertexName());
                    DJV_ASSERT(fragmentPath.get() == shader->getFragmentName());
                    _print(shader->getVertexSource());
                    _print(shader->getFragmentSource());
                }
                
                try
                {
                    auto shader = Render::Shader::create(FileSystem::Path(), FileSystem::Path());
                    DJV_ASSERT(false);
                }
                catch (const std::exception& e)
                {
                    _print(Error::format(e.what()));
                }
            }
        }

    } // namespace AVTest
} // namespace djv

