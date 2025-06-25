// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/ToolBar.h>

namespace djv
{
    namespace app
    {
        //! Tools tool bar.
        class ToolsToolBar : public feather_tk::ToolBar
        {
            FEATHER_TK_NON_COPYABLE(ToolsToolBar);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::map<std::string, std::shared_ptr<feather_tk::Action> >&,
                const std::shared_ptr<IWidget>& parent);

            ToolsToolBar() = default;

        public:
            ~ToolsToolBar();

            static std::shared_ptr<ToolsToolBar> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::map<std::string, std::shared_ptr<feather_tk::Action> >&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
