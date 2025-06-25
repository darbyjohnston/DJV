// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/ToolBar.h>

namespace djv
{
    namespace app
    {
        class ViewActions;

        //! View tool bar.
        class ViewToolBar : public feather_tk::ToolBar
        {
            FEATHER_TK_NON_COPYABLE(ViewToolBar);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<ViewActions>&,
                const std::shared_ptr<IWidget>& parent);

            ViewToolBar() = default;

        public:
            ~ViewToolBar();

            static std::shared_ptr<ViewToolBar> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<ViewActions>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
