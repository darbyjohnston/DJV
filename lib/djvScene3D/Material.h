// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene3D/Enum.h>

#include <djvImage/Color.h>

#include <memory>

namespace djv
{
    namespace Render3D
    {
        class IMaterial;

    } // namespace Render3D

    namespace System
    {
        class Context;

    } // namespace System

    namespace Scene3D
    {
        //! Base class for materials.
        class IMaterial : public std::enable_shared_from_this<IMaterial>
        {
            DJV_NON_COPYABLE(IMaterial);

        protected:
            IMaterial();

        public:
            virtual ~IMaterial() = 0;

            virtual std::shared_ptr<Render3D::IMaterial> createMaterial(const std::shared_ptr<System::Context>&) = 0;
        };

        //! Default material.
        class DefaultMaterial : public IMaterial
        {
        protected:
            DefaultMaterial();

        public:
            static std::shared_ptr<DefaultMaterial> create();

            std::shared_ptr<Render3D::IMaterial> createMaterial(const std::shared_ptr<System::Context>&) override;

            const Image::Color& getAmbient() const;
            const Image::Color& getDiffuse() const;
            const Image::Color& getEmission() const;
            const Image::Color& getSpecular() const;
            float getShine() const;
            float getTransparency() const;
            float getReflectivity() const;
            bool hasDisableLighting() const;

            void setAmbient(const Image::Color&);
            void setDiffuse(const Image::Color&);
            void setEmission(const Image::Color&);
            void setSpecular(const Image::Color&);
            void setShine(float);
            void setTransparency(float);
            void setReflectivity(float);
            void setDisableLighting(bool);

        private:
            Image::Color _ambient = Image::Color::RGB_F32(0.F, 0.F, 0.4F);
            Image::Color _diffuse = Image::Color::RGB_F32(.5F, .5F, .5F);
            Image::Color _emission = Image::Color::RGB_F32(0.F, 0.F, 0.F);
            Image::Color _specular = Image::Color::RGB_F32(1.F, 1.F, 1.F);
            float _shine = 0.F;
            float _transparency = 0.F;
            float _reflectivity = 0.F;
            bool _disableLighting = false;
        };

    } // namespace Scene3D
} // namespace djv

#include <djvScene3D/MaterialInline.h>
