// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! Keyboard shortcut settings widget.
        class KeyboardShortcutSettingsWidget : public UIComponents::Settings::IWidget
        {
            DJV_NON_COPYABLE(KeyboardShortcutSettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            KeyboardShortcutSettingsWidget();

        public:
            static std::shared_ptr<KeyboardShortcutSettingsWidget> create(const std::shared_ptr<System::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

