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
            //! This class provides a FFmpeg settings widget.
            class FFmpegWidget : public IWidget
            {
                DJV_NON_COPYABLE(FFmpegWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                FFmpegWidget();

            public:
                static std::shared_ptr<FFmpegWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsName() const override;
                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

            protected:
                void _initEvent(System::Event::Init&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

