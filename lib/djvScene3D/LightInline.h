// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene3D
    {
        inline bool ILight::isEnabled() const
        {
            return _enabled;
        }

        inline float ILight::getIntensity() const
        {
            return _intensity;
        }

        inline void ILight::setEnabled(bool value)
        {
            _enabled = value;
        }

        inline void ILight::setIntensity(float value)
        {
            _intensity = value;
        }

        inline const glm::vec3& HemisphereLight::getUp() const
        {
            return _up;
        }

        inline const Image::Color& HemisphereLight::getTopColor() const
        {
            return _topColor;
        }

        inline const Image::Color& HemisphereLight::getBottomColor() const
        {
            return _bottomColor;
        }

        inline void HemisphereLight::setUp(const glm::vec3& value)
        {
            _up = value;
        }

        inline void HemisphereLight::setTopColor(const Image::Color& value)
        {
            _topColor = value;
        }

        inline void HemisphereLight::setBottomColor(const Image::Color& value)
        {
            _bottomColor = value;
        }

        inline std::string HemisphereLight::getClassName() const
        {
            return "HemisphereLight";
        }

        inline const glm::vec3& DirectionalLight::getDirection() const
        {
            return _direction;
        }

        inline void DirectionalLight::setDirection(const glm::vec3& value)
        {
            _direction = value;
        }

        inline std::string DirectionalLight::getClassName() const
        {
            return "DirectionalLight";
        }

        inline std::string PointLight::getClassName() const
        {
            return "PointLight";
        }

        inline float SpotLight::getConeAngle() const
        {
            return _coneAngle;
        }

        inline const glm::vec3& SpotLight::getDirection() const
        {
            return _direction;
        }

        inline void SpotLight::setConeAngle(float value)
        {
            _coneAngle = value;
        }

        inline void SpotLight::setDirection(const glm::vec3& value)
        {
            _direction = value;
        }

        inline std::string SpotLight::getClassName() const
        {
            return "SpotLight";
        }

    } // namespace Scene3D
} // namespace djv
