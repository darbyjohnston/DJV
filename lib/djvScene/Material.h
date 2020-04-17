// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene/Enum.h>

#include <djvAV/Color.h>

#include <memory>

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            class IMaterial;

        } // namespace Render3D
    } // namespace AV

    namespace Core
    {
        class Context;

    } // namespace AV

    namespace Scene
    {
        //! This class provides the base functionality for materials.
        class IMaterial : public std::enable_shared_from_this<IMaterial>
        {
            DJV_NON_COPYABLE(IMaterial);

        protected:
            IMaterial();

        public:
            virtual ~IMaterial() = 0;

            virtual std::shared_ptr<AV::Render3D::IMaterial> createMaterial(const std::shared_ptr<Core::Context>&) = 0;
        };

        //! This class provides a default material.
        class DefaultMaterial : public IMaterial
        {
        protected:
            DefaultMaterial();

        public:
            static std::shared_ptr<DefaultMaterial> create();

            std::shared_ptr<AV::Render3D::IMaterial> createMaterial(const std::shared_ptr<Core::Context>&) override;

            const AV::Image::Color& getAmbient() const;
            const AV::Image::Color& getDiffuse() const;
            const AV::Image::Color& getEmission() const;
            const AV::Image::Color& getSpecular() const;
            float getShine() const;
            float getTransparency() const;
            float getReflectivity() const;
            bool hasDisableLighting() const;

            void setAmbient(const AV::Image::Color&);
            void setDiffuse(const AV::Image::Color&);
            void setEmission(const AV::Image::Color&);
            void setSpecular(const AV::Image::Color&);
            void setShine(float);
            void setTransparency(float);
            void setReflectivity(float);
            void setDisableLighting(bool);

        private:
            AV::Image::Color _ambient = AV::Image::Color::RGB_F32(0.F, 0.F, 0.4F);
            AV::Image::Color _diffuse = AV::Image::Color::RGB_F32(.5F, .5F, .5F);
            AV::Image::Color _emission = AV::Image::Color::RGB_F32(0.F, 0.F, 0.F);
            AV::Image::Color _specular = AV::Image::Color::RGB_F32(1.F, 1.F, 1.F);
            float _shine = 0.F;
            float _transparency = 0.F;
            float _reflectivity = 0.F;
            bool _disableLighting = false;
        };

    } // namespace Scene
} // namespace djv

#include <djvScene/MaterialInline.h>
