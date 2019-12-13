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
            AV::Render3D::DepthBufferMode       depthBufferMode = AV::Render3D::DepthBufferMode::Reverse;
            AV::OpenGL::OffscreenDepthType      depthBufferType = AV::OpenGL::OffscreenDepthType::_32;
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
            virtual ~SceneWidget();

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

