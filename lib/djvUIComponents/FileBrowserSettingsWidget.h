// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            //! This class provides a file browser settings widget.
            class SettingsWidget : public UI::ISettingsWidget
            {
                DJV_NON_COPYABLE(SettingsWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                SettingsWidget();

            public:
                static std::shared_ptr<SettingsWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsName() const override;
                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

            private:
                DJV_PRIVATE();
            };

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv

