// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

namespace djv
{
    namespace ViewApp
    {
        class NUXWidget;

        //! This class provides the new user experience system.
        class NUXSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(NUXSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            NUXSystem();

        public:
            static std::shared_ptr<NUXSystem> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<NUXWidget> createNUXWidget();

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

