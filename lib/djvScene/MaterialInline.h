// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {
        inline IMaterial::IMaterial()
        {}

        inline IMaterial::~IMaterial()
        {}

        inline DefaultMaterial::DefaultMaterial()
        {}

        inline const Image::Color& DefaultMaterial::getAmbient() const
        {
            return _ambient;
        }

        inline const Image::Color& DefaultMaterial::getDiffuse() const
        {
            return _diffuse;
        }

        inline const Image::Color& DefaultMaterial::getEmission() const
        {
            return _emission;
        }

        inline const Image::Color& DefaultMaterial::getSpecular() const
        {
            return _specular;
        }

        inline float DefaultMaterial::getShine() const
        {
            return _shine;
        }

        inline float DefaultMaterial::getTransparency() const
        {
            return _transparency;
        }

        inline float DefaultMaterial::getReflectivity() const
        {
            return _reflectivity;
        }

        inline bool DefaultMaterial::hasDisableLighting() const
        {
            return _disableLighting;
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

    } // namespace Scene
} // namespace djv
