// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/ImageData.h>
#include <djvAV/Render3D.h>
#include <djvAV/Render3DMaterial.h>

#include <glm/mat4x4.hpp>

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            class Render;

        } // namespace Render3D
    } // namespace AV

    namespace Core
    {
        class Context;

    } // namespace Core

    //! This namespace provides scene functionality.
    namespace Scene
    {
        class ICamera;
        class IMaterial;
        class IPrimitive;
        class Scene;

        //! This struct provides render options.
        struct RenderOptions
        {
            std::shared_ptr<ICamera>            camera;
            AV::Image::Size                     size;
            Core::FloatRange                    clip;
            AV::Render3D::DefaultMaterialMode   shaderMode      = AV::Render3D::DefaultMaterialMode::Default;
            AV::Render3D::DepthBufferMode       depthBufferMode = AV::Render3D::DepthBufferMode::Reverse;

            bool operator == (const RenderOptions&) const;
        };

        //! This class provides a renderer.
        class Render : public std::enable_shared_from_this<Render>
        {
            DJV_NON_COPYABLE(Render);
            void _init(const std::shared_ptr<Core::Context>&);
            Render();

        public:
            static std::shared_ptr<Render> create(const std::shared_ptr<Core::Context>&);

            void setScene(const std::shared_ptr<Scene>&);

            void render(
                const std::shared_ptr<AV::Render3D::Render>&,
                const RenderOptions&);

            size_t getPrimitivesCount() const;
            size_t getPointCount() const;

        private:
            AV::Image::Color _getColor(const std::shared_ptr<IPrimitive>&) const;
            std::shared_ptr<IMaterial> _getMaterial(const std::shared_ptr<IPrimitive>&) const;
            const glm::mat4x4& _getCurrentTransform() const;
            void _pushTransform(const glm::mat4x4&);
            void _popTransform();
            void _prePass(
                const std::shared_ptr<IPrimitive>&,
                const std::shared_ptr<Core::Context>&);

            DJV_PRIVATE();
        };

    } // namespace Scene
} // namespace djv
