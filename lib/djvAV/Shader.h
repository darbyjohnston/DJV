//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#pragma once

#include <djvAV/AV.h>

#include <memory>
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

            private:
                std::pair<std::string, std::string> _vertex;
                std::pair<std::string, std::string> _fragment;
            };

        } // namespace Render
    } // namespace AV
} // namespace djv
