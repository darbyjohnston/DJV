// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Color.h>

#include <glm/mat4x4.hpp>

#include <map>
#include <memory>

namespace djv
{
    namespace System
    {
        class Context;
    
    } // namespace System

    namespace GL
    {
        class Shader;

    } // namespace GL

    namespace Render3D
    {
        class ILight;

        //! Light binding data.
        struct BindData
        {
            std::vector<std::shared_ptr<ILight> > lights;
        };

        //! Per-primitive binding data.
        struct PrimitiveBindData
        {
            glm::mat4x4 model;
            glm::mat4x4 camera;
            Image::Color color;
        };

        //! Base class for materials.
        class IMaterial : public std::enable_shared_from_this<IMaterial>
        {
            DJV_NON_COPYABLE(IMaterial);
            
        protected:
            void _init(
                const std::string& vertex,
                const std::string& fragment,
                const std::shared_ptr<System::Context>&);

            IMaterial();

        public:
            virtual ~IMaterial() = 0;

            virtual void bind(const BindData&);
            virtual void primitiveBind(const PrimitiveBindData&);

            const std::shared_ptr<GL::Shader>& getShader() const;

        protected:
            Image::Color _color;
            std::shared_ptr<GL::Shader> _shader;
        };

        //! Unlit solid colored material.
        class SolidColorMaterial : public IMaterial
        {
            DJV_NON_COPYABLE(SolidColorMaterial);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            SolidColorMaterial();

        public:
            ~SolidColorMaterial() override;

            static std::shared_ptr<SolidColorMaterial> create(const std::shared_ptr<System::Context>&);

            void primitiveBind(const PrimitiveBindData&) override;

        private:
            std::map<std::string, GLint> _locations;
        };

        //! Default material modes.
        enum class DefaultMaterialMode
        {
            Default,
            Unlit,
            Normals,
            UVs,

            Count,
            First = Default
        };
        DJV_ENUM_HELPERS(DefaultMaterialMode);

        //! Default material.
        class DefaultMaterial : public IMaterial
        {
            DJV_NON_COPYABLE(DefaultMaterial);

        protected:
            void _init(const std::shared_ptr<System::Context>&);

            DefaultMaterial();

        public:
            ~DefaultMaterial() override;

            static std::shared_ptr<DefaultMaterial> create(const std::shared_ptr<System::Context>&);

            void setMode(DefaultMaterialMode);
            void setAmbient(const Image::Color&);
            void setDiffuse(const Image::Color&);
            void setEmission(const Image::Color&);
            void setSpecular(const Image::Color&);
            void setShine(float);
            void setTransparency(float);
            void setReflectivity(float);
            void setDisableLighting(bool);

            void bind(const BindData&) override;
            void primitiveBind(const PrimitiveBindData&) override;

        private:
            DefaultMaterialMode _mode = DefaultMaterialMode::Default;
            Image::Color _ambient = Image::Color::RGB_F32(0.F, 0.F, 0.4F);
            Image::Color _diffuse = Image::Color::RGB_F32(.5F, .5F, .5F);
            Image::Color _emission = Image::Color::RGB_F32(0.F, 0.F, 0.F);
            Image::Color _specular = Image::Color::RGB_F32(1.F, 1.F, 1.F);
            float _shine = 0.F;
            float _transparency = 0.F;
            float _reflectivity = 0.F;
            bool _disableLighting = false;
            std::map<std::string, GLint> _locations;
        };

    } // namespace Render3D

    DJV_ENUM_SERIALIZE_HELPERS(Render3D::DefaultMaterialMode);

} // namespace djv

#include <djvRender3D/MaterialInline.h>
