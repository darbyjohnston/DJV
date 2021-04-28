// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Color.h>

#include <djvSystem/ISystem.h>

#include <glm/mat4x4.hpp>

namespace djv
{
    namespace Render3D
    {
        //! Base class for lights.
        class ILight : public std::enable_shared_from_this<ILight>
        {
            DJV_NON_COPYABLE(ILight);

        protected:
            ILight();

        public:
            virtual ~ILight() = 0;

            float getIntensity() const;

            void setIntensity(float);

        private:
            float _intensity = 1.F;
        };

        //! Hemisphere light.
        class HemisphereLight : public ILight
        {
        protected:
            HemisphereLight();

        public:
            static std::shared_ptr<HemisphereLight> create();

            const glm::vec3& getUp() const;
            const Image::Color& getTopColor() const;
            const Image::Color& getBottomColor() const;

            void setUp(const glm::vec3&);
            void setTopColor(const Image::Color&);
            void setBottomColor(const Image::Color&);

        private:
            glm::vec3 _up = glm::vec3(0.F, 1.F, 0.F);
            Image::Color _topColor = Image::Color::RGB_F32(1.F, .8F, .7F);
            Image::Color _bottomColor = Image::Color::RGB_F32(.4F, .4F, .8F);
        };

        //! Directional light.
        class DirectionalLight : public ILight
        {
        protected:
            DirectionalLight();

        public:
            static std::shared_ptr<DirectionalLight> create();

            const glm::vec3& getDirection() const;

            void setDirection(const glm::vec3&);

        private:
            glm::vec3 _direction = glm::vec3(-1.F, -1.F, -1.F);
        };

        //! Point light.
        class PointLight : public ILight
        {
        protected:
            PointLight();

        public:
            static std::shared_ptr<PointLight> create();

            const glm::vec3& getPosition() const;

            void setPosition(const glm::vec3&);

        private:
            glm::vec3 _position = glm::vec3(0.F, 0.F, 0.F);
        };

        //! Spot light.
        class SpotLight : public ILight
        {
        protected:
            SpotLight();

        public:
            static std::shared_ptr<SpotLight> create();

            float getConeAngle() const;
            const glm::vec3& getDirection() const;
            const glm::vec3& getPosition() const;

            void setConeAngle(float);
            void setDirection(const glm::vec3&);
            void setPosition(const glm::vec3&);

        private:
            float _coneAngle = 90.F;
            glm::vec3 _direction = glm::vec3(-1.F, -1.F, -1.F);
            glm::vec3 _position = glm::vec3(0.F, 0.F, 0.F);
        };

    } // namespace Render3D
} // namespace djv

#include <djvRender3D/LightInline.h>
