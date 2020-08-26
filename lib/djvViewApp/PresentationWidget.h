// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides a presentation widget.
        class PresentationWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(PresentationWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PresentationWidget();

        public:
            ~PresentationWidget() override;

            static std::shared_ptr<PresentationWidget> create(const std::shared_ptr<Core::Context>&);

            void setCloseCallback(const std::function<void(void)>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _buttonPressEvent(Core::Event::ButtonPress&) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

