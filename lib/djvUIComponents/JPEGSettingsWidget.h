// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            //! This class provides a JPEG settings widget.
            class JPEGWidget : public IWidget
            {
                DJV_NON_COPYABLE(JPEGWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                JPEGWidget();

            public:
                static std::shared_ptr<JPEGWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsName() const override;
                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

                void setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>&) override;

            protected:
                void _initEvent(System::Event::Init&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

