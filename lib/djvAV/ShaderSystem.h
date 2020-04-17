// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <djvCore/ISystem.h>

namespace djv
{
    namespace AV
    {
        namespace OpenGL
        {
            class Shader;
        
        } // namespace OpenGL

        namespace Render
        {
            //! This class provides a shader system.
            class ShaderSystem : public Core::ISystem
            {
                DJV_NON_COPYABLE(ShaderSystem);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                ShaderSystem();

            public:
                ~ShaderSystem();

                static std::shared_ptr<ShaderSystem> create(const std::shared_ptr<Core::Context>&);

                //! Throws:
                //! - std::exception
                std::shared_ptr<OpenGL::Shader> getShader(
                    const std::string& vertex,
                    const std::string& fragment);

            private:
                DJV_PRIVATE();
            };

        } // namespace Render
    } // namespace AV
} // namespace djv
