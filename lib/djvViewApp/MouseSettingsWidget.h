// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides a mouse scroll wheel settings widget.
        class MouseScrollWheelSettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(MouseScrollWheelSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            MouseScrollWheelSettingsWidget();

        public:
            static std::shared_ptr<MouseScrollWheelSettingsWidget> create(const std::shared_ptr<Core::Context>&);

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

    } // namespace ViewApp
} // namespace djv

