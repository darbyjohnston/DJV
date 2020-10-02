// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace UIComponents
    {
        //! This class provides a style size widget.
        class SizeWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(SizeWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            SizeWidget();

        public:
            static std::shared_ptr<SizeWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();
            void _currentItemUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a style palette widget.
        class PaletteWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(PaletteWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            PaletteWidget();

        public:
            static std::shared_ptr<PaletteWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            
            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();
            void _currentItemUpdate();

            DJV_PRIVATE();
        };

        namespace Settings
        {
            //! This class provides a style settings widget.
            class StyleWidget : public IWidget
            {
                DJV_NON_COPYABLE(StyleWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                StyleWidget();

            public:
                static std::shared_ptr<StyleWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

                void setLabelSizeGroup(const std::weak_ptr<UI::Text::LabelSizeGroup>&) override;

            protected:
                void _initEvent(System::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

