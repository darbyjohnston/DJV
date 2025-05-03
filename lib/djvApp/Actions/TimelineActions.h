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

        //! Timeline actions.
        class TimelineActions : public IActions
        {
            DTK_NON_COPYABLE(TimelineActions);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&);

            TimelineActions();

        public:
            ~TimelineActions();

            static std::shared_ptr<TimelineActions> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&);

        private:
            void _thumbnailsSizeUpdate();

            DTK_PRIVATE();
        };
    }
}
