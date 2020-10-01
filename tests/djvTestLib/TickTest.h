// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

#include <djvCore/Time.h>

namespace djv
{
    namespace Test
    {
        //! This class provides the interface for confidence tests which need to be ticked.
        class ITickTest : public ITest
        {
        public:
            ITickTest(
                const std::string& name,
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            virtual ~ITickTest() = 0;

        protected:
            void _tickFor(const Core::Time::Duration&);
        };
        
    } // namespace Test
} // namespace djv

