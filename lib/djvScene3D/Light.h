// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene3D/IPrimitive.h>

#include <glm/vec3.hpp>

#include <memory>

namespace djv
{
    namespace Scene3D
    {
        //! This class provides the base functionality for lights.
        class ILight : public IPrimitive
        {
            DJV_NON_COPYABLE(ILight);

        protected:
            ILight();

        public:
            virtual ~ILight() = 0;

            bool isEnabled() const;
            float getIntensity() const;

            void setEnabled(bool);
            void setIntensity(float);

        private:
            bool _enabled = true;
            float _intensity = 1.F;
        };

        //! This class provides a hemisphere light.
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

            std::string getClassName() const override;

        private:
            glm::vec3 _up = glm::vec3(0.F, 1.F, 0.F);
            Image::Color _topColor = Image::Color::RGB_F32(1.F, 1.F, 1.F);
            Image::Color _bottomColor = Image::Color::RGB_F32(.4F, .4F, .8F);
        };

        //! This class provides a directional light.
        class DirectionalLight : public ILight
        {
        protected:
            DirectionalLight();

        public:
            static std::shared_ptr<DirectionalLight> create();

            const glm::vec3& getDirection() const;

            void setDirection(const glm::vec3&);

            std::string getClassName() const override;

        private:
            glm::vec3 _direction = glm::vec3(-1.F, -1.F, -1.F);
        };

        //! This class provides a point light.
        class PointLight : public ILight
        {
        protected:
            PointLight();

        public:
            static std::shared_ptr<PointLight> create();

            std::string getClassName() const override;
        };

        //! This class provides a spot light.
        class SpotLight : public ILight
        {
        protected:
            SpotLight();

        public:
            static std::shared_ptr<SpotLight> create();

            float getConeAngle() const;
            const glm::vec3& getDirection() const;

            void setConeAngle(float);
            void setDirection(const glm::vec3&);

            std::string getClassName() const override;

        private:
            float _coneAngle = 90.F;
            glm::vec3 _direction = glm::vec3(-1.F, -1.F, -1.F);
        };

    } // namespace Scene3D
} // namespace djv

#include <djvScene3D/LightInline.h>
