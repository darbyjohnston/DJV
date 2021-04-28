// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            //! I/O threads settings widget.
            class IOThreadsWidget : public IWidget
            {
                DJV_NON_COPYABLE(IOThreadsWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                IOThreadsWidget();

            public:
                static std::shared_ptr<IOThreadsWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

            protected:
                void _initEvent(System::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

