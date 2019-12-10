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

#include <djvAV/Render3DMaterial.h>

#include <djvAV/OpenGLShader.h>
#include <djvAV/Render3DLight.h>
#include <djvAV/ShaderSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            void IMaterial::_init(
                const std::string& vertex,
                const std::string& fragment,
                const std::shared_ptr<Context>& context)
            {
                auto shaderSystem = context->getSystemT<AV::Render::ShaderSystem>();
                _shader = shaderSystem->getShader(vertex, fragment);
                auto program = _shader->getProgram();
                _mvpLoc = glGetUniformLocation(program, "transform.mvp");
            }

            IMaterial::IMaterial()
            {}

            IMaterial::~IMaterial()
            {}

            void IMaterial::bind()
            {}

            void DefaultMaterial::_init(const std::shared_ptr<Context>& context)
            {
                IMaterial::_init("djvAVRender3DDefaultVertex.glsl", "djvAVRender3DDefaultFragment.glsl", context);
                auto program = _shader->getProgram();
                _modeLoc = glGetUniformLocation(program, "defaultMaterial.mode");
                _ambientLoc = glGetUniformLocation(program, "defaultMaterial.ambient");
                _diffuseLoc = glGetUniformLocation(program, "defaultMaterial.diffuse");
                _emissionLoc = glGetUniformLocation(program, "defaultMaterial.emission");
                _specularLoc = glGetUniformLocation(program, "defaultMaterial.specular");
                _shineLoc = glGetUniformLocation(program, "defaultMaterial.shine");
                _transparencyLoc = glGetUniformLocation(program, "defaultMaterial.transparency");
                _reflectivityLoc = glGetUniformLocation(program, "defaultMaterial.reflectivity");
                _disableLightingLoc = glGetUniformLocation(program, "defaultMaterial.disableLighting");
            }

            DefaultMaterial::DefaultMaterial()
            {}

            DefaultMaterial::~DefaultMaterial()
            {}

            std::shared_ptr<DefaultMaterial> DefaultMaterial::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<DefaultMaterial>(new DefaultMaterial);
                out->_init(context);
                return out;
            }

            void DefaultMaterial::setMode(DefaultMaterialMode value)
            {
                _mode = value;
            }

            void DefaultMaterial::setAmbient(const AV::Image::Color& value)
            {
                _ambient = value;
            }

            void DefaultMaterial::setDiffuse(const AV::Image::Color& value)
            {
                _diffuse = value;
            }

            void DefaultMaterial::setEmission(const AV::Image::Color& value)
            {
                _emission = value;
            }

            void DefaultMaterial::setSpecular(const AV::Image::Color& value)
            {
                _specular = value;
            }

            void DefaultMaterial::setShine(float value)
            {
                _shine = value;
            }

            void DefaultMaterial::setTransparency(float value)
            {
                _transparency = value;
            }

            void DefaultMaterial::setReflectivity(float value)
            {
                _reflectivity = value;
            }

            void DefaultMaterial::setDisableLighting(bool value)
            {
                _disableLighting = value;
            }

            void DefaultMaterial::bind()
            {
                _shader->setUniform(_modeLoc, static_cast<int>(_mode));
                _shader->setUniform(_ambientLoc, _ambient);
                _shader->setUniform(_diffuseLoc, _diffuse);
                _shader->setUniform(_emissionLoc, _emission);
                _shader->setUniform(_specularLoc, _specular);
                _shader->setUniform(_shineLoc, _shine);
                _shader->setUniform(_transparencyLoc, _transparency);
                _shader->setUniform(_reflectivityLoc, _reflectivity);
                _shader->setUniform(_disableLightingLoc, _disableLighting);
            }

        } // namespace Render3D
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Render3D,
        DefaultMaterialMode,
        DJV_TEXT("Default"),
        DJV_TEXT("Unlit"),
        DJV_TEXT("Normals"),
        DJV_TEXT("UVs"));


} // namespace djv
