// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace UIComponents
    {
        //! This class provides a language widget.
        class LanguageWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(LanguageWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            LanguageWidget();

        public:
            static std::shared_ptr<LanguageWidget> create(const std::shared_ptr<System::Context>&);

            float getHeightForWidth(float) const override;

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        namespace Settings
        {
            //! This class provides a language settings widget.
            class LanguageWidget : public IWidget
            {
                DJV_NON_COPYABLE(LanguageWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                LanguageWidget();

            public:
                static std::shared_ptr<LanguageWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

            private:
                void _initEvent(System::Event::Init&) override;

                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

