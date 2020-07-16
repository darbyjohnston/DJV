// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        class Menu;

        //! This class provides a menu that is shown by clicking a button.
        class PopupMenu : public Widget
        {
            DJV_NON_COPYABLE(PopupMenu);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PopupMenu();

        public:
            ~PopupMenu() override;

            static std::shared_ptr<PopupMenu> create(const std::shared_ptr<Core::Context>&);

            void setMenu(const std::shared_ptr<Menu>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

