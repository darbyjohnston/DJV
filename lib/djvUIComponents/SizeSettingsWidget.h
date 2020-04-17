// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a size widget.
        class SizeWidget : public Widget
        {
            DJV_NON_COPYABLE(SizeWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            SizeWidget();

        public:
            static std::shared_ptr<SizeWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();
            void _currentItemUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a size settings widget.
        class SizeSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(SizeSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            SizeSettingsWidget();

        public:
            static std::shared_ptr<SizeSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

