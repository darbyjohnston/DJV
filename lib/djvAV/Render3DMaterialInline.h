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

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            inline IMaterial::IMaterial()
            {}

            inline IMaterial::~IMaterial()
            {}

            inline void IMaterial::bind(const BindData&)
            {}

            inline void IMaterial::primitiveBind(const PrimitiveBindData&)
            {}

            inline const std::shared_ptr<OpenGL::Shader>& IMaterial::getShader() const
            {
                return _shader;
            }

            inline SolidColorMaterial::SolidColorMaterial()
            {}

            inline SolidColorMaterial::~SolidColorMaterial()
            {}

            inline DefaultMaterial::DefaultMaterial()
            {}

            inline DefaultMaterial::~DefaultMaterial()
            {}

            inline void DefaultMaterial::setMode(DefaultMaterialMode value)
            {
                _mode = value;
            }

            inline void DefaultMaterial::setAmbient(const AV::Image::Color& value)
            {
                _ambient = value;
            }

            inline void DefaultMaterial::setDiffuse(const AV::Image::Color& value)
            {
                _diffuse = value;
            }

            inline void DefaultMaterial::setEmission(const AV::Image::Color& value)
            {
                _emission = value;
            }

            inline void DefaultMaterial::setSpecular(const AV::Image::Color& value)
            {
                _specular = value;
            }

            inline void DefaultMaterial::setShine(float value)
            {
                _shine = value;
            }

            inline void DefaultMaterial::setTransparency(float value)
            {
                _transparency = value;
            }

            inline void DefaultMaterial::setReflectivity(float value)
            {
                _reflectivity = value;
            }

            inline void DefaultMaterial::setDisableLighting(bool value)
            {
                _disableLighting = value;
            }

        } // namespace Render3D
    } // namespace AV
} // namespace djv
