// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a mouse settings widget.
        class MouseSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(MouseSettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MouseSettingsWidget();

        public:
            static std::shared_ptr<MouseSettingsWidget> create(const std::shared_ptr<System::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<LabelSizeGroup>&) override;

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

