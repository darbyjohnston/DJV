// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the input system.
        class InputSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(InputSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            InputSystem();

        public:
            ~InputSystem() override;

            static std::shared_ptr<InputSystem> create(const std::shared_ptr<Core::Context>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

