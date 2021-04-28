// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/Widget.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        class Media;
        class ViewWidget;

        //! Pointer state.
        enum class PointerState
        {
            None,
            Start,
            Move,
            End
        };

        //! Pointer data.
        struct PointerData
        {
            PointerData();
            PointerData(
                PointerState               state,
                const glm::vec2&           pos,
                const std::map<int, bool>& buttons,
                int                        key,
                int                        keyModifiers);

            PointerState        state        = PointerState::None;
            glm::vec2           pos          = glm::vec2(0.F, 0.F);
            std::map<int, bool> buttons;
            int                 key          = 0;
            int                 keyModifiers = 0;

            bool operator == (const PointerData&) const;
        };

        //! Scroll data.
        struct ScrollData
        {
            ScrollData();
            ScrollData(
                const glm::vec2& delta,
                int              key,
                int              keyModifiers);

            glm::vec2           delta = glm::vec2(0.F, 0.F);
            int                 key = 0;
            int                 keyModifiers = 0;

            bool operator == (const ScrollData&) const;
        };

        //! Media widget.
        class MediaWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(MediaWidget);

        protected:
            void _init(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<System::Context>&);
            MediaWidget();

        public:
            ~MediaWidget() override;

            static std::shared_ptr<MediaWidget> create(
                const std::shared_ptr<Media>&,
                const std::shared_ptr<System::Context>&);

            const std::shared_ptr<Media>& getMedia() const;

            const std::shared_ptr<ViewWidget>& getViewWidget() const;

            std::shared_ptr<Core::Observer::IValueSubject<PointerData> > observeHover() const;
            std::shared_ptr<Core::Observer::IValueSubject<PointerData> > observeDrag() const;
            std::shared_ptr<Core::Observer::IValueSubject<ScrollData> > observeScroll() const;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            void _imageUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

