// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a style size widget.
        class SizeWidget : public Widget
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
        class PaletteWidget : public Widget
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

        //! This class provides a style settings widget.
        class StyleSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(StyleSettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            StyleSettingsWidget();

        public:
            static std::shared_ptr<StyleSettingsWidget> create(const std::shared_ptr<System::Context>&);

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

