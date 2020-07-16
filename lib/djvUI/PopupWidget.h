// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a popup widget.
        class PopupWidget : public Widget
        {
            DJV_NON_COPYABLE(PopupWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PopupWidget();

        public:
            ~PopupWidget() override;

            static std::shared_ptr<PopupWidget> create(const std::shared_ptr<Core::Context>&);

            void addChild(const std::shared_ptr<IObject>&) override;
            void removeChild(const std::shared_ptr<IObject>&) override;
            void clearChildren() override;

            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _buttonPressEvent(Core::Event::ButtonPress&) override;
            void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

