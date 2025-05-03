// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Actions/IActions.h>

namespace djv
{
    namespace app
    {
        //! Compare actions.
        class CompareActions : public IActions
        {
            DTK_NON_COPYABLE(CompareActions);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&);

            CompareActions();

        public:
            ~CompareActions();

            static std::shared_ptr<CompareActions> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&);

        private:
            DTK_PRIVATE();
        };
    }
}
