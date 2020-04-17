// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a palette widget.
        class PaletteWidget : public Widget
        {
            DJV_NON_COPYABLE(PaletteWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PaletteWidget();

        public:
            static std::shared_ptr<PaletteWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            
            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();
            void _currentItemUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a palette settings widget.
        class PaletteSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(PaletteSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            PaletteSettingsWidget();

        public:
            static std::shared_ptr<PaletteSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

