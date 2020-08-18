// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a time units widget.
        class TimeUnitsWidget : public Widget
        {
            DJV_NON_COPYABLE(TimeUnitsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            TimeUnitsWidget();

        public:
            static std::shared_ptr<TimeUnitsWidget> create(const std::shared_ptr<Core::Context>&);

            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();
            void _currentItemUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a time settings widget.
        class TimeSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(TimeSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            TimeSettingsWidget();

        public:
            static std::shared_ptr<TimeSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

