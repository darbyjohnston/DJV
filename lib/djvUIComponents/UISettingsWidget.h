// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a tooltips settings widget.
        class TooltipsSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(TooltipsSettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            TooltipsSettingsWidget();

        public:
            static std::shared_ptr<TooltipsSettingsWidget> create(const std::shared_ptr<System::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

