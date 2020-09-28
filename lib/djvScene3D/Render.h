// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvRender3D/Render.h>
#include <djvRender3D/Material.h>

#include <djvImage/ImageData.h>

#include <glm/mat4x4.hpp>

namespace djv
{
    namespace Render3D
    {
        class Render;

    } // namespace Render3D

    namespace System
    {
        class Context;

    } // namespace System

    namespace Scene3D
    {
        class ICamera;
        class IMaterial;
        class IPrimitive;
        class Scene;

        //! This struct provides render options.
        struct RenderOptions
        {
            std::shared_ptr<ICamera>      camera;
            Image::Size                   size;
            Math::FloatRange              clip;
            Render3D::DefaultMaterialMode shaderMode      = Render3D::DefaultMaterialMode::Default;
            Render3D::DepthBufferMode     depthBufferMode = Render3D::DepthBufferMode::Reverse;

            bool operator == (const RenderOptions&) const;
        };

        //! This class provides a renderer.
        class Render : public std::enable_shared_from_this<Render>
        {
            DJV_NON_COPYABLE(Render);
            void _init(const std::shared_ptr<System::Context>&);
            Render();

        public:
            static std::shared_ptr<Render> create(const std::shared_ptr<System::Context>&);

            void setScene(const std::shared_ptr<Scene>&);

            void render(
                const std::shared_ptr<Render3D::Render>&,
                const RenderOptions&);

            size_t getPrimitivesCount() const;
            size_t getPointCount() const;

        private:
            Image::Color _getColor(const std::shared_ptr<IPrimitive>&) const;
            std::shared_ptr<IMaterial> _getMaterial(const std::shared_ptr<IPrimitive>&) const;
            const glm::mat4x4& _getCurrentTransform() const;
            void _pushTransform(const glm::mat4x4&);
            void _popTransform();
            void _prePass(
                const std::shared_ptr<IPrimitive>&,
                const std::shared_ptr<System::Context>&);

            DJV_PRIVATE();
        };

    } // namespace Scene3D
} // namespace djv
