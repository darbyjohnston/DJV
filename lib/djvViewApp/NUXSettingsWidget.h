// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the new user experience settings widget.
        class NUXSettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(NUXSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            NUXSettingsWidget();

        public:
            static std::shared_ptr<NUXSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

