// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Actions/IActions.h>

namespace djv
{
    namespace app
    {
        //! Color actions.
        class ColorActions : public IActions
        {
            FTK_NON_COPYABLE(ColorActions);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

            ColorActions();

        public:
            ~ColorActions();

            static std::shared_ptr<ColorActions> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

        private:
            FTK_PRIVATE();
        };
    }
}
