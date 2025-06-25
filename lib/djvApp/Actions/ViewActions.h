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

        //! View actions.
        //!
        //! \todo Add an action for toggling the UI visibility.
        class ViewActions : public IActions
        {
            FEATHER_TK_NON_COPYABLE(ViewActions);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&);

            ViewActions();

        public:
            ~ViewActions();

            static std::shared_ptr<ViewActions> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&);

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}
