// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Actions/IActions.h>

namespace djv
{
    namespace app
    {
        class MainWindow;

        //! Window actions.
        class WindowActions : public IActions
        {
            FEATHER_TK_NON_COPYABLE(WindowActions);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&);

            WindowActions();

        public:
            ~WindowActions();

            static std::shared_ptr<WindowActions> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
