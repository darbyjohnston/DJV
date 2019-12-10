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

#pragma once

#include <djvAV/Color.h>

namespace djv
{
    namespace Core
    {
        class Context;
    
    } // namespace Core

    namespace AV
    {
        namespace OpenGL
        {
            class Shader;

        } // namespace OpenGL

        namespace Render3D
        {
            //! This class provides the base functionality for materials.
            class IMaterial : public std::enable_shared_from_this<IMaterial>
            {
                DJV_NON_COPYABLE(IMaterial);
                
            protected:
                void _init(
                    const std::string& vertex,
                    const std::string& fragment,
                    const std::shared_ptr<Core::Context>&);

                IMaterial();

            public:
                virtual ~IMaterial() = 0;

                virtual void bind();

                const std::shared_ptr<OpenGL::Shader>& getShader() const;
                GLint getMVPLoc() const;

            protected:
                std::shared_ptr<OpenGL::Shader> _shader;
                GLint _mvpLoc = 0;
            };

            //! This enumeration provides the default material modes.
            enum class DefaultMaterialMode
            {
                Default,
                Unlit,
                Normals,
                UVs,

                Count,
                First = Default
            };
            DJV_ENUM_HELPERS(DefaultMaterialMode);

            //! This class provides a default material.
            class DefaultMaterial : public IMaterial
            {
                DJV_NON_COPYABLE(DefaultMaterial);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);

                DefaultMaterial();

            public:
                ~DefaultMaterial() override;

                static std::shared_ptr<DefaultMaterial> create(const std::shared_ptr<Core::Context>&);

                void setMode(DefaultMaterialMode);
                void setAmbient(const AV::Image::Color&);
                void setDiffuse(const AV::Image::Color&);
                void setEmission(const AV::Image::Color&);
                void setSpecular(const AV::Image::Color&);
                void setShine(float);
                void setTransparency(float);
                void setReflectivity(float);
                void setDisableLighting(bool);

                void bind() override;

            private:
                DefaultMaterialMode _mode = DefaultMaterialMode::Default;
                AV::Image::Color _ambient = AV::Image::Color::RGB_F32(0.F, 0.F, 0.4F);
                AV::Image::Color _diffuse = AV::Image::Color::RGB_F32(.5F, .5F, .5F);
                AV::Image::Color _emission = AV::Image::Color::RGB_F32(0.F, 0.F, 0.F);
                AV::Image::Color _specular = AV::Image::Color::RGB_F32(1.F, 1.F, 1.F);
                float _shine = 0.F;
                float _transparency = 0.F;
                float _reflectivity = 0.F;
                bool _disableLighting = false;
                GLint _modeLoc = 0;
                GLint _ambientLoc = 0;
                GLint _diffuseLoc = 0;
                GLint _emissionLoc = 0;
                GLint _specularLoc = 0;
                GLint _shineLoc = 0;
                GLint _transparencyLoc = 0;
                GLint _reflectivityLoc = 0;
                GLint _disableLightingLoc = 0;
            };

        } // namespace Render3D
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Render3D::DefaultMaterialMode);

} // namespace djv

#include <djvAV/Render3DMaterialInline.h>
