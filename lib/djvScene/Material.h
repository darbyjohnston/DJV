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
