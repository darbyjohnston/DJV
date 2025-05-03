// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <dtk/ui/ToolBar.h>

namespace djv
{
    namespace app
    {
        //! Compare tool bar.
        class CompareToolBar : public dtk::ToolBar
        {
            DTK_NON_COPYABLE(CompareToolBar);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::map<std::string, std::shared_ptr<dtk::Action> >&,
                const std::shared_ptr<IWidget>& parent);

            CompareToolBar() = default;

        public:
            ~CompareToolBar();

            static std::shared_ptr<CompareToolBar> create(
                const std::shared_ptr<dtk::Context>&,
                const std::map<std::string, std::shared_ptr<dtk::Action> >&,
                const std::shared_ptr<IWidget>& parent = nullptr);
        };
    }
}
