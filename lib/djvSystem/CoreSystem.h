// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

namespace djv
{
    namespace System
    {
        //! This class provides the core systems.
        class CoreSystem : public ISystem
        {
            DJV_NON_COPYABLE(CoreSystem);

        protected:
            void _init(const std::string& argv0, const std::shared_ptr<Context>&);
            CoreSystem();

        public:
            ~CoreSystem() override;

            static std::shared_ptr<CoreSystem> create(const std::string& argv0, const std::shared_ptr<Context>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace System
} // namespace djv
