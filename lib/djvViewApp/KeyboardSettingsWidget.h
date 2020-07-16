// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides a keyboard shortcut settings widget.
        class KeyboardShortcutSettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(KeyboardShortcutSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            KeyboardShortcutSettingsWidget();

        public:
            static std::shared_ptr<KeyboardShortcutSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

