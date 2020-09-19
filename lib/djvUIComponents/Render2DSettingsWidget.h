// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a 2D renderer image settings widget.
        class Render2DImageSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(Render2DImageSettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            Render2DImageSettingsWidget();

        public:
            static std::shared_ptr<Render2DImageSettingsWidget> create(const std::shared_ptr<System::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides a 2D renderer text settings widget.
        class Render2DTextSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(Render2DTextSettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            Render2DTextSettingsWidget();

        public:
            static std::shared_ptr<Render2DTextSettingsWidget> create(const std::shared_ptr<System::Context>&);

            std::string getSettingsName() const override;
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

