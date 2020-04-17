// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Color.h>
#include <djvAV/ImageData.h>

#include <djvCore/ISystem.h>

#include <glm/mat4x4.hpp>

namespace djv
{
    namespace AV
    {
        namespace Geom
        {
            class PointList;
            class TriangleMesh;
        
        } // namespace Geom

        namespace OpenGL
        {
            class Shader;

        } // namespace OpenGL

        namespace Render3D
        {
            class ICamera;
            class ILight;
            class IMaterial;

            //! This enumeration provides depth buffer modes.
            enum class DepthBufferMode
            {
                Standard,
                Reverse,

                Count,
                First = Standard
            };
            DJV_ENUM_HELPERS(DepthBufferMode);

            //! This struct provides render options.
            struct RenderOptions
            {
                std::shared_ptr<ICamera>    camera;
                Image::Size                 size;
                Core::FloatRange            clip;
                DepthBufferMode             depthBufferMode = DepthBufferMode::Reverse;
            };

            //! This class provides a 3D render system.
            class Render : public Core::ISystem
            {
                DJV_NON_COPYABLE(Render);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Render();

            public:
                ~Render();

                static std::shared_ptr<Render> create(const std::shared_ptr<Core::Context>&);

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

                void setColor(const AV::Image::Color&);
                void setMaterial(const std::shared_ptr<IMaterial>&);

                ///@}

                //! \name Lights
                ///@{

                void addLight(const std::shared_ptr<ILight>&);

                ///@}

                //! \name Primitives
                ///@{

                void drawPoints(const std::vector<std::shared_ptr<AV::Geom::PointList> >&);

                void drawPolyLine(const std::shared_ptr<AV::Geom::PointList>&);
                void drawPolyLines(const std::vector<std::shared_ptr<AV::Geom::PointList> >&);

                void drawTriangleMesh(const Geom::TriangleMesh&);
                void drawTriangleMeshes(const std::vector<Geom::TriangleMesh>&);
                void drawTriangleMeshes(const std::vector<std::shared_ptr<Geom::TriangleMesh> >&);

                ///@}

            private:
                DJV_PRIVATE();
            };

        } // namespace Render3D
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Render3D::DepthBufferMode);

} // namespace djv
