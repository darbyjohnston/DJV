// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/ShortcutData.h>
#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a widget for editing shortcuts.
        class ShortcutsWidget : public Widget
        {
            DJV_NON_COPYABLE(ShortcutsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ShortcutsWidget();

        public:
            static std::shared_ptr<ShortcutsWidget> create(const std::shared_ptr<Core::Context>&);

            void setShortcuts(const ShortcutDataMap&);
            void setShortcutsCallback(const std::function<void(const ShortcutDataMap&)>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init &) override;

        private:
            void _popup(
                const std::shared_ptr<Widget>& button,
                const std::string&,
                const ShortcutDataPair&);
            
            void _shortcutsUpdate();
            void _itemsUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

