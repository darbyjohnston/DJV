// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ShortcutData.h>
#include <djvUI/Widget.h>

namespace djv
{
    namespace UIComponents
    {
        //! This class provides a widget for editing keyboard shortcuts.
        class ShortcutsWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ShortcutsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ShortcutsWidget();

        public:
            static std::shared_ptr<ShortcutsWidget> create(const std::shared_ptr<System::Context>&);

            void setShortcuts(const UI::ShortcutDataMap&);
            void setShortcutsCallback(const std::function<void(const UI::ShortcutDataMap&)>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UIComponents
} // namespace djv

