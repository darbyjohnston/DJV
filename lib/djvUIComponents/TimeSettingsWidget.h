// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace UIComponents
    {
        //! This class provides a time units widget.
        class TimeUnitsWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(TimeUnitsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            TimeUnitsWidget();

        public:
            static std::shared_ptr<TimeUnitsWidget> create(const std::shared_ptr<System::Context>&);

            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();
            void _currentItemUpdate();

            DJV_PRIVATE();
        };

        namespace Settings
        {
            //! This class provides a time settings widget.
            class TimeWidget : public IWidget
            {
                DJV_NON_COPYABLE(TimeWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                TimeWidget();

            public:
                static std::shared_ptr<TimeWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

                void setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>&) override;

            protected:
                void _initEvent(System::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

