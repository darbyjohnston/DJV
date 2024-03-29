// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/SettingsIWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! Memory cache enabled widget.
        class MemoryCacheEnabledWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(MemoryCacheEnabledWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MemoryCacheEnabledWidget();

        public:
            ~MemoryCacheEnabledWidget() override;

            static std::shared_ptr<MemoryCacheEnabledWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

        //! Memory cache size widget.
        class MemoryCacheSizeWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(MemoryCacheSizeWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MemoryCacheSizeWidget();

        public:
            ~MemoryCacheSizeWidget() override;

            static std::shared_ptr<MemoryCacheSizeWidget> create(const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

        //! Memory settings widget.
        class MemorySettingsWidget : public UIComponents::Settings::IWidget
        {
            DJV_NON_COPYABLE(MemorySettingsWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MemorySettingsWidget();

        public:
            static std::shared_ptr<MemorySettingsWidget> create(const std::shared_ptr<System::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        protected:
            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

