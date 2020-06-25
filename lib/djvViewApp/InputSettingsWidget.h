// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the input settings widget.
        class ScrollWheelSettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(ScrollWheelSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ScrollWheelSettingsWidget();

        public:
            static std::shared_ptr<ScrollWheelSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a shortcuts settings widget.
        class ShortcutsSettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(ShortcutsSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ShortcutsSettingsWidget();

        public:
            static std::shared_ptr<ShortcutsSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

