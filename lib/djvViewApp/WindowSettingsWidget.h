// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the window geometry settings widget.
        class WindowGeometrySettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(WindowGeometrySettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            WindowGeometrySettingsWidget();

        public:
            ~WindowGeometrySettingsWidget() override;

            static std::shared_ptr<WindowGeometrySettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

        //! This class provides the fullscreen monitor settings widget.
        class FullscreenMonitorSettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(FullscreenMonitorSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            FullscreenMonitorSettingsWidget();

        public:
            ~FullscreenMonitorSettingsWidget() override;

            static std::shared_ptr<FullscreenMonitorSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides the auto-hide settings widget.
        class AutoHideSettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(AutoHideSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            AutoHideSettingsWidget();

        public:
            static std::shared_ptr<AutoHideSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };
        
        //! This class provides the background image settings widget.
        class BackgroundImageSettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(BackgroundImageSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            BackgroundImageSettingsWidget();

        public:
            ~BackgroundImageSettingsWidget() override;

            static std::shared_ptr<BackgroundImageSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init&) override;
            void _updateEvent(Core::Event::Update&) override;

        private:
            void _widgetUpdate();
            void _imageUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

