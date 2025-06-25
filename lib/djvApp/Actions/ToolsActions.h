// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Actions/IActions.h>

namespace djv
{
    namespace app
    {
        //! Tools actions.
        class ToolsActions : public IActions
        {
            FEATHER_TK_NON_COPYABLE(ToolsActions);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&);

            ToolsActions();

        public:
            ~ToolsActions();

            static std::shared_ptr<ToolsActions> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
