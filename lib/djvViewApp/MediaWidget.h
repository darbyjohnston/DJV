//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewApp/Enum.h>

#include <djvUI/MDIWidget.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        class ImageView;
        class Media;

        //! This enumeration provides the pointer state.
        enum class PointerState
        {
            Start,
            Move,
            End
        };

        //! This truct provides pointer information.
        struct PointerData
        {
            PointerData();
            PointerData(PointerState, const glm::vec2&, const std::map<int, bool>& buttons);

            PointerState state = PointerState::Start;
            glm::vec2 pos = glm::vec2(0.F, 0.F);
            std::map<int, bool> buttons;

            bool operator == (const PointerData&) const;
        };

        //! This class provides a media widget.
        class MediaWidget : public UI::MDI::IWidget
        {
            DJV_NON_COPYABLE(MediaWidget);

        protected:
            void _init(const std::shared_ptr<Media>&, const std::shared_ptr<Core::Context>&);
            MediaWidget();

        public:
            ~MediaWidget() override;

            static std::shared_ptr<MediaWidget> create(const std::shared_ptr<Media>&, const std::shared_ptr<Core::Context>&);

            const std::shared_ptr<Media>& getMedia() const;

            const std::shared_ptr<ImageView>& getImageView() const;

            void fitWindow();

            std::shared_ptr<Core::IValueSubject<PointerData> > observeHover() const;
            std::shared_ptr<Core::IValueSubject<PointerData> > observeDrag() const;

        protected:
            float _getTitleBarHeight() const;
            float _getPlaybackHeight() const;

            virtual std::map<UI::MDI::Handle, std::vector<Core::BBox2f> > _getHandles() const override;
            void _setMaximize(float) override;
            void _setActiveWidget(bool) override;

            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _textUpdateEvent(Core::Event::TextUpdate&) override;

        private:
            void _widgetUpdate();
            void _imageUpdate();
            void _speedUpdate();
            void _realSpeedUpdate();
            void _audioUpdate();
            void _opacityUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

