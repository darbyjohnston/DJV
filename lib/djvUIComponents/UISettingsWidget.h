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
            void _init(const std::shared_ptr<Core::Context>&);
            TooltipsSettingsWidget();

        public:
            static std::shared_ptr<TooltipsSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        protected:
            void _initEvent(Core::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

        //! This class provides a scroll settings widget.
        class ScrollSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(ScrollSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ScrollSettingsWidget();

        public:
            static std::shared_ptr<ScrollSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        protected:
            void _initEvent(Core::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

