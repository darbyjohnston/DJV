// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvScene3D/Camera.h>
#include <djvScene3D/Render.h>

#include <djvRender3D/Render.h>

#include <djvGL/OffscreenBuffer.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Scene3D
    {
        class Scene;

    } // namespace Scene3D

    namespace UIComponents
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

        //! This struct provides render options.
        struct SceneRenderOptions
        {
            Render3D::DefaultMaterialMode shaderMode      = Render3D::DefaultMaterialMode::Default;
#if defined(DJV_GL_ES2)
            Render3D::DepthBufferMode     depthBufferMode = Render3D::DepthBufferMode::Standard;
            GL::OffscreenDepthType        depthBufferType = GL::OffscreenDepthType::_24;
#else // DJV_GL_ES2
            Render3D::DepthBufferMode     depthBufferMode = Render3D::DepthBufferMode::Reverse;
            GL::OffscreenDepthType        depthBufferType = GL::OffscreenDepthType::_32;
#endif // DJV_GL_ES2
            GL::OffscreenSampling         multiSampling   = GL::OffscreenSampling::None;

            bool operator == (const SceneRenderOptions&) const;
        };

        //! This class provides a scene view widget.
        class SceneWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(SceneWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            SceneWidget();

        public:
            ~SceneWidget() override;

            static std::shared_ptr<SceneWidget> create(const std::shared_ptr<System::Context>&);

            //! \name Scene
            ///@{

            const std::shared_ptr<Scene3D::Scene>& getScene() const;

            void setScene(const std::shared_ptr<Scene3D::Scene>&);

            ///@}

            //! \name View
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<Scene3D::PolarCameraData> > observeCameraData() const;

            void setCameraData(const Scene3D::PolarCameraData&);

            void frameView();

            ///@}

            //! \name Options
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<SceneRotate> > observeSceneRotate() const;
            std::shared_ptr<Core::Observer::IValueSubject<SceneRenderOptions> > observeRenderOptions() const;

            void setSceneRotate(SceneRotate);
            void setRenderOptions(const SceneRenderOptions&);

            ///@}

            //! \name Information
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<Math::BBox3f> > observeBBox() const;
            std::shared_ptr<Core::Observer::IValueSubject<size_t> > observePrimitivesCount() const;
            std::shared_ptr<Core::Observer::IValueSubject<size_t> > observePointCount() const;

            ///@}

        protected:
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint&) override;
            void _pointerMoveEvent(System::Event::PointerMove&) override;
            void _buttonPressEvent(System::Event::ButtonPress&) override;
            void _buttonReleaseEvent(System::Event::ButtonRelease&) override;

            void _updateEvent(System::Event::Update&) override;

        private:
            void _sceneUpdate();

            DJV_PRIVATE();
        };

    } // namespace UIComponents
} // namespace djv

