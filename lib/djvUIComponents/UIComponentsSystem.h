// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

namespace djv
{
    //! User interface components.
    namespace UIComponents
    {
        //! UI components system.
        class UIComponentsSystem : public System::ISystem
        {
            DJV_NON_COPYABLE(UIComponentsSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            UIComponentsSystem();

        public:
            ~UIComponentsSystem() override;

            static std::shared_ptr<UIComponentsSystem> create(const std::shared_ptr<System::Context>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace UIComponents
} // namespace djv
