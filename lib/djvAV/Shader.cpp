// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Shader.h>

#include <djvCore/FileIO.h>
#include <djvCore/Path.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Render
        {
            ShaderError::ShaderError(const std::string& what) :
                std::runtime_error(what)
            {}
            
            Shader::Shader()
            {}

            Shader::~Shader()
            {}

            std::shared_ptr<Shader> Shader::create(
                const std::string& vertex,
                const std::string& fragment)
            {
                auto out = std::shared_ptr<Shader>(new Shader);
                out->_vertex.second = vertex;
                out->_fragment.second = fragment;
                return out;
            }

            std::shared_ptr<Shader> Shader::create(
                const FileSystem::Path& vertex,
                const FileSystem::Path& fragment)
            {
                auto out = std::shared_ptr<Shader>(new Shader);
                try
                {
                    auto fileIO = FileSystem::FileIO::create();
                    fileIO->open(std::string(vertex), FileSystem::FileIO::Mode::Read);
                    out->_vertex.second = FileSystem::readContents(fileIO);
                    out->_vertex.first = vertex.get();
                    fileIO->open(std::string(fragment), FileSystem::FileIO::Mode::Read);
                    out->_fragment.second = FileSystem::readContents(fileIO);
                    out->_fragment.first = fragment.get();
                }
                catch (const std::exception& e)
                {
                    throw ShaderError(e.what());
                }
                return out;
            }

            const std::string& Shader::getVertexName() const
            {
                return _vertex.first;
            }

            const std::string& Shader::getVertexSource() const
            {
                return _vertex.second;
            }

            const std::string& Shader::getFragmentName() const
            {
                return _fragment.first;
            }

            const std::string& Shader::getFragmentSource() const
            {
                return _fragment.second;
            }

            void Shader::setVertexName(const std::string& value)
            {
                _vertex.first = value;
            }
            
            void Shader::setFragmentName(const std::string& value)
            {
                _fragment.first = value;
            }

        } // namespace Render
    } // namespace AV
} // namespace djv
