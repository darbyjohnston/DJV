// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides a memory cache enabled widget.
        class MemoryCacheEnabledWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(MemoryCacheEnabledWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            MemoryCacheEnabledWidget();

        public:
            ~MemoryCacheEnabledWidget() override;

            static std::shared_ptr<MemoryCacheEnabledWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

        private:
            DJV_PRIVATE();
        };

        //! This class provides a memory cache size widget.
        class MemoryCacheSizeWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(MemoryCacheSizeWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            MemoryCacheSizeWidget();

        public:
            ~MemoryCacheSizeWidget() override;

            static std::shared_ptr<MemoryCacheSizeWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

        //! This class provides a memory settings widget.
        class MemorySettingsWidget : public UI::ISettingsWidget
        {
            DJV_NON_COPYABLE(MemorySettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            MemorySettingsWidget();

        public:
            static std::shared_ptr<MemorySettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

            void setLabelSizeGroup(const std::weak_ptr<UI::LabelSizeGroup>&) override;

        protected:
            void _initEvent(Core::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

