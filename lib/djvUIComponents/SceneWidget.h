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

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Scene
    {
        class Scene;

    } // namespace Scene

    namespace UI
    {
        //! This enum provides the scene rotation.
        enum class SceneRotate
        {
            None,
            X90,
            X_90,
            Y90,
            Y_90,
            Z90,
            Z_90
        };

        //! This struct provides camera information.
        struct CameraInfo
        {
            float fov = 0.F;
            djv::Core::FloatRange clip;
            float distance = 0.F;
            float latitude = 0.F;
            float longitude = 0.F;

            bool operator == (const CameraInfo&) const;
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

            std::shared_ptr<Core::IValueSubject<CameraInfo> > observeCameraInfo() const;
            void setCameraInfo(const CameraInfo&);

            void frameView();

            std::shared_ptr<Core::IValueSubject<Core::BBox3f> > observeBBox() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observePrimitivesCount() const;
            std::shared_ptr<Core::IValueSubject<size_t> > observeTriangleCount() const;

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
} // namespace djv

