// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Render3D
    {
        inline void IMaterial::bind(const BindData&)
        {
            // Default implementation does nothing.
        }

        inline void IMaterial::primitiveBind(const PrimitiveBindData&)
        {
            // Default implementation does nothing.
        }

        inline const std::shared_ptr<GL::Shader>& IMaterial::getShader() const
        {
            return _shader;
        }

        inline void DefaultMaterial::setMode(DefaultMaterialMode value)
        {
            _mode = value;
        }

        inline void DefaultMaterial::setAmbient(const Image::Color& value)
        {
            _ambient = value;
        }

        inline void DefaultMaterial::setDiffuse(const Image::Color& value)
        {
            _diffuse = value;
        }

        inline void DefaultMaterial::setEmission(const Image::Color& value)
        {
            _emission = value;
        }

        inline void DefaultMaterial::setSpecular(const Image::Color& value)
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
} // namespace djv
