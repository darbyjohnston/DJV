// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/Color.h>
#include <djvImage/Info.h>

#include <djvSystem/ISystem.h>

#include <glm/mat4x4.hpp>

namespace djv
{
    namespace Geom
    {
        class PointList;
        class TriangleMesh;
    
    } // namespace Geom

    namespace GL
    {
        class Shader;

    } // namespace GL

    //! Three-dimensional rendering.
    namespace Render3D
    {
        class ICamera;
        class ILight;
        class IMaterial;

        //! Depth buffer modes.
        enum class DepthBufferMode
        {
            Standard,
            Reverse,

            Count,
            First = Standard
        };
        DJV_ENUM_HELPERS(DepthBufferMode);

        //! Render options.
        struct RenderOptions
        {
            std::shared_ptr<ICamera>    camera;
            Image::Size                 size;
            Math::FloatRange            clip;
            DepthBufferMode             depthBufferMode = DepthBufferMode::Reverse;
        };

        //! Three-dimensional renderer.
        class Render : public System::ISystem
        {
            DJV_NON_COPYABLE(Render);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            Render();

        public:
            ~Render();

            static std::shared_ptr<Render> create(const std::shared_ptr<System::Context>&);

            //! \name Begin and End
            ///@{

            void beginFrame(const RenderOptions&);
            void endFrame();

            ///@}

            //! \name Transform
            ///@{

            const glm::mat4x4& getCurrentTransform() const;
            const glm::mat4x4& getCurrentInverseTransform() const;

            void pushTransform(const glm::mat4x4&);
            void popTransform();

            ///@}

            //! \name Color and Material
            ///@{

            void setColor(const Image::Color&);
            void setMaterial(const std::shared_ptr<IMaterial>&);

            ///@}

            //! \name Lights
            ///@{

            void addLight(const std::shared_ptr<ILight>&);

            ///@}

            //! \name Primitives
            ///@{

            void drawPoints(const std::vector<std::shared_ptr<Geom::PointList> >&);

            void drawPolyLine(const std::shared_ptr<Geom::PointList>&);
            void drawPolyLines(const std::vector<std::shared_ptr<Geom::PointList> >&);

            void drawTriangleMesh(const Geom::TriangleMesh&);
            void drawTriangleMeshes(const std::vector<Geom::TriangleMesh>&);
            void drawTriangleMeshes(const std::vector<std::shared_ptr<Geom::TriangleMesh> >&);

            ///@}

        private:
            DJV_PRIVATE();
        };

    } // namespace Render3D

    DJV_ENUM_SERIALIZE_HELPERS(Render3D::DepthBufferMode);

} // namespace djv
