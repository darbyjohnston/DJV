// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            //! This class provides a file browser settings widget.
            class FileBrowserWidget : public IWidget
            {
                DJV_NON_COPYABLE(FileBrowserWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                FileBrowserWidget();

            public:
                static std::shared_ptr<FileBrowserWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsName() const override;
                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

