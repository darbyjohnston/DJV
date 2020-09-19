// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvGL/GL.h>

#include <djvSystem/ISystem.h>

namespace djv
{
    namespace GL
    {
        class Shader;
        
        //! This class provides a shader system.
        class ShaderSystem : public System::ISystem
        {
            DJV_NON_COPYABLE(ShaderSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ShaderSystem();

        public:
            ~ShaderSystem();

            static std::shared_ptr<ShaderSystem> create(const std::shared_ptr<System::Context>&);

            //! Throws:
            //! - std::exception
            std::shared_ptr<Shader> getShader(
                const std::string& vertexSource,
                const std::string& fragmentSource);

        private:
            DJV_PRIVATE();
        };

    } // namespace GL
} // namespace djv
