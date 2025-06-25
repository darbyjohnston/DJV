// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/IToolWidget.h>

namespace djv
{
    namespace app
    {
        //! Settings tool.
        class SettingsTool : public IToolWidget
        {
            FEATHER_TK_NON_COPYABLE(SettingsTool);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            SettingsTool();

        public:
            virtual ~SettingsTool();

            static std::shared_ptr<SettingsTool> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
