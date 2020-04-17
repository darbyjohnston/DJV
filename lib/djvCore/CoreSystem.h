// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/ISystem.h>

namespace djv
{
    namespace Core
    {
        //! This class provides the core systems.
        class CoreSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(CoreSystem);

        protected:
            void _init(const std::string & argv0, const std::shared_ptr<Context>&);
            CoreSystem();

        public:
            ~CoreSystem() override;

            static std::shared_ptr<CoreSystem> create(const std::string & argv0, const std::shared_ptr<Context>&);

        private:
            DJV_PRIVATE();
        };

    } // namespace Core
} // namespace djv
