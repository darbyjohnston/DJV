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
        //! This class provides an interface for confidence tests which need to be ticked.
        class ITickTest : public ITest
        {
        public:
            ITickTest(const std::string & name, const std::shared_ptr<Core::Context>&);
            virtual ~ITickTest() = 0;

        protected:
            void _tickFor(const Core::Time::Duration&);
        };
        
    } // namespace Test
} // namespace djv

