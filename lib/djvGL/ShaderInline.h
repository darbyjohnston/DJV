// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace GL
    {
        inline const std::string& Shader::getVertexName() const
        {
            return _vertexSource.first;
        }

        inline const std::string& Shader::getVertexSource() const
        {
            return _vertexSource.second;
        }

        inline const std::string& Shader::getFragmentName() const
        {
            return _fragmentSource.first;
        }

        inline const std::string& Shader::getFragmentSource() const
        {
            return _fragmentSource.second;
        }

        inline void Shader::setVertexName(const std::string& value)
        {
            _vertexSource.first = value;
        }
        
        inline void Shader::setFragmentName(const std::string& value)
        {
            _fragmentSource.first = value;
        }

        inline GLuint Shader::getProgram() const
        {
            return _program;
        }
        
    } // namespace GL
} // namespace djv
