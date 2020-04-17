// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
            None,
            Start,
            Move,
            End
        };

        //! This truct provides pointer information.
        struct PointerData
        {
            PointerData();
            PointerData(PointerState, const glm::vec2&, const std::map<int, bool>& buttons);

            PointerState state = PointerState::None;
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
            std::shared_ptr<Core::IValueSubject<glm::vec2> > observeScroll() const;

        protected:
            float _getTitleBarHeight() const;
            float _getPlaybackHeight() const;

            virtual std::map<UI::MDI::Handle, std::vector<Core::BBox2f> > _getHandles() const override;
            void _setMaximize(float) override;
            void _setActiveWidget(bool) override;

            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

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

