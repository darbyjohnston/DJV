// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/ToolBar.h>

namespace djv
{
    namespace app
    {
        //! Window tool bar.
        class WindowToolBar : public feather_tk::ToolBar
        {
            FEATHER_TK_NON_COPYABLE(WindowToolBar);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::map<std::string, std::shared_ptr<feather_tk::Action> >&,
                const std::shared_ptr<IWidget>& parent);

            WindowToolBar() = default;

        public:
            ~WindowToolBar();

            static std::shared_ptr<WindowToolBar> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::map<std::string, std::shared_ptr<feather_tk::Action> >&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
