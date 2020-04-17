// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Drawer.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the settings drawer.
        class SettingsDrawer : public UI::Drawer
        {
            DJV_NON_COPYABLE(SettingsDrawer);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            SettingsDrawer();

        public:
            ~SettingsDrawer() override;

            static std::shared_ptr<SettingsDrawer> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _openStart(void) override;
            void _closeEnd(void) override;

            void _initLayoutEvent(Core::Event::InitLayout&) override;

            void _initEvent(Core::Event::Init &) override;

        private:
            void _textUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

