// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            //! This class provides a 2D renderer image settings widget.
            class Render2DImageWidget : public IWidget
            {
                DJV_NON_COPYABLE(Render2DImageWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Render2DImageWidget();

            public:
                static std::shared_ptr<Render2DImageWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsName() const override;
                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

            protected:
                void _initEvent(System::Event::Init&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

            //! This class provides a 2D renderer text settings widget.
            class Render2DTextWidget : public IWidget
            {
                DJV_NON_COPYABLE(Render2DTextWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Render2DTextWidget();

            public:
                static std::shared_ptr<Render2DTextWidget> create(const std::shared_ptr<System::Context>&);

                std::string getSettingsName() const override;
                std::string getSettingsGroup() const override;
                std::string getSettingsSortKey() const override;

            protected:
                void _initEvent(System::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

