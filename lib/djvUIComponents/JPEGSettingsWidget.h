// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a JPEG settings widget.
        class JPEGSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(JPEGSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            JPEGSettingsWidget();

        public:
            static std::shared_ptr<JPEGSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setSizeGroup(const std::weak_ptr<LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

