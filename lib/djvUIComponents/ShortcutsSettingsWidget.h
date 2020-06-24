// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/ISettingsWidget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a shortcuts settings widget.
        class ShortcutsSettingsWidget : public ISettingsWidget
        {
            DJV_NON_COPYABLE(ShortcutsSettingsWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ShortcutsSettingsWidget();

        public:
            static std::shared_ptr<ShortcutsSettingsWidget> create(const std::shared_ptr<Core::Context>&);

            std::string getSettingsName() const override;
            std::string getSettingsGroup() const override;
            std::string getSettingsSortKey() const override;

        protected:
            void _initEvent(Core::Event::Init &) override;

        private:
            void _itemsUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

