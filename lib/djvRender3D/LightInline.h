// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Render3D
    {
        inline ILight::ILight()
        {}

        inline ILight::~ILight()
        {}

        inline float ILight::getIntensity() const
        {
            return _intensity;
        }

        inline void ILight::setIntensity(float value)
        {
            _intensity = value;
        }

        inline HemisphereLight::HemisphereLight()
        {}

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

        inline DirectionalLight::DirectionalLight()
        {}

        inline const glm::vec3& DirectionalLight::getDirection() const
        {
            return _direction;
        }

        inline void DirectionalLight::setDirection(const glm::vec3& value)
        {
            _direction = value;
        }

        inline PointLight::PointLight()
        {}

        inline const glm::vec3& PointLight::getPosition() const
        {
            return _position;
        }

        inline void PointLight::setPosition(const glm::vec3& value)
        {
            _position = value;
        }

        inline SpotLight::SpotLight()
        {}

        inline float SpotLight::getConeAngle() const
        {
            return _coneAngle;
        }

        inline const glm::vec3& SpotLight::getDirection() const
        {
            return _direction;
        }

        inline const glm::vec3& SpotLight::getPosition() const
        {
            return _position;
        }

        inline void SpotLight::setConeAngle(float value)
        {
            _coneAngle = value;
        }

        inline void SpotLight::setDirection(const glm::vec3& value)
        {
            _direction = value;
        }

        inline void SpotLight::setPosition(const glm::vec3& value)
        {
            _position = value;
        }

    } // namespace Render3D
} // namespace djv
