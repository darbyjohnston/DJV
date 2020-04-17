// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a language widget.
        class LanguageWidget : public Widget
        {
            DJV_NON_COPYABLE(LanguageWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            LanguageWidget();

        public:
            static std::shared_ptr<LanguageWidget> create(const std::shared_ptr<Core::Context>&);

            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a language settings widget.
        class LanguageSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(LanguageSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            LanguageSettingsWidget();

        public:
            static std::shared_ptr<LanguageSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

