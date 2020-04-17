// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ISystem.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a UI system.
        class UIComponentsSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(UIComponentsSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            UIComponentsSystem();

        public:
            ~UIComponentsSystem() override;

            static std::shared_ptr<UIComponentsSystem> create(const std::shared_ptr<Core::Context>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv
