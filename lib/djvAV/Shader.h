// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class Path;

        } // namespace FileSystem
    } // namespace Core

    namespace AV
    {
        namespace Render
        {
            //! This class provides a shader error.
            class ShaderError : public std::runtime_error
            {
            public:
                explicit ShaderError(const std::string&);
            };
            
            //! This class provides shader source code.
            class Shader
            {
                DJV_NON_COPYABLE(Shader);

            protected:
                Shader();

            public:
                ~Shader();

                static std::shared_ptr<Shader> create(const std::string & vertex, const std::string & fragment);

                //! Throws:
                //! - ShaderError
                static std::shared_ptr<Shader> create(const Core::FileSystem::Path & vertex, const Core::FileSystem::Path & fragment);

                const std::string & getVertexName() const;
                const std::string & getVertexSource() const;
                const std::string & getFragmentName() const;
                const std::string & getFragmentSource() const;

                void setVertexName(const std::string&);
                void setFragmentName(const std::string&);

            private:
                std::pair<std::string, std::string> _vertex;
                std::pair<std::string, std::string> _fragment;
            };

        } // namespace Render
    } // namespace AV
} // namespace djv
