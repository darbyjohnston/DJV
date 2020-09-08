// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvScene/Camera.h>
#include <djvScene/Render.h>

#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render3D.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Scene
    {
        class Scene;

    } // namespace Scene

    namespace UI
    {
        //! This enumeration provides the scene rotation.
        enum class SceneRotate
        {
            None,
            X90,
            X_90,
            Y90,
            Y_90,
            Z90,
            Z_90,

            Count,
            First = None
        };
        DJV_ENUM_HELPERS(SceneRotate);

        //! This struct provides render options.
        struct SceneRenderOptions
        {
            AV::Render3D::DefaultMaterialMode   shaderMode      = AV::Render3D::DefaultMaterialMode::Default;
#if defined(DJV_OPENGL_ES2)
            AV::Render3D::DepthBufferMode       depthBufferMode = AV::Render3D::DepthBufferMode::Standard;
            AV::OpenGL::OffscreenDepthType      depthBufferType = AV::OpenGL::OffscreenDepthType::_24;
#else // DJV_OPENGL_ES2
            AV::Render3D::DepthBufferMode       depthBufferMode = AV::Render3D::DepthBufferMode::Reverse;
            AV::OpenGL::OffscreenDepthType      depthBufferType = AV::OpenGL::OffscreenDepthType::_32;
#endif // DJV_OPENGL_ES2
            AV::OpenGL::OffscreenSampling       multiSampling   = AV::OpenGL::OffscreenSampling::None;

            bool operator == (const SceneRenderOptions&) const;
        };

        //! This class provides a scene view widget.
        class SceneWidget : public Widget
        {
            DJV_NON_COPYABLE(SceneWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            SceneWidget();

        public:
            ~SceneWidget() override;

            static std::shared_ptr<SceneWidget> create(const std::shared_ptr<Core::Context>&);

            const std::shared_ptr<Scene::Scene>& getScene() const;
            void setScene(const std::shared_ptr<Scene::Scene>&);

            std::shared_ptr<Core::IValueSubject<SceneRotate> > observeSceneRotate() const;
            void setSceneRotate(SceneRotate);

            std::shared_ptr<Core::IValueSubject<Scene::PolarCameraData> > observeCameraData() const;
            void setCameraData(const Scene::PolarCameraData&);

            std::shared_ptr<Core::IValueSubject<SceneRenderOptions> > observeRenderOptions() const;
            void setRenderOptions(const SceneRenderOptions&);

            void frameView();

            std::shared_ptr<Core::IValueSubject<Core::BBox3f> > observeBBox() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observePrimitivesCount() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observePointCount() const;

        protected:
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;
            void _pointerMoveEvent(Core::Event::PointerMove&) override;
            void _buttonPressEvent(Core::Event::ButtonPress&) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;

            void _updateEvent(Core::Event::Update&) override;

        private:
            void _sceneUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI

    DJV_ENUM_SERIALIZE_HELPERS(UI::SceneRotate);

} // namespace djv

