// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the settings widget.
        class SettingsWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(SettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            SettingsWidget();

        public:
            ~SettingsWidget() override;

            static std::shared_ptr<SettingsWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init &) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

