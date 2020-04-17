// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/ValueObserverTest.h>

#include <djvCore/ValueObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        ValueObserverTest::ValueObserverTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ValueObserverTest", context)
        {}
        
        void ValueObserverTest::run()
        {
            int value = 0;
            auto subject = ValueSubject<int>::create(value);
            {
                int value2 = 0;
                auto observer = ValueObserver<int>::create(
                    subject,
                    [&value2](int value)
                    {
                        value2 = value;
                    });
                DJV_ASSERT(1 == subject->getObserversCount());

                DJV_ASSERT(!subject->setIfChanged(value));
                ++value;
                subject->setAlways(value);
                DJV_ASSERT(subject->get() == value2);
                ++value;
                DJV_ASSERT(subject->setIfChanged(value));
                DJV_ASSERT(!subject->setIfChanged(value));
                DJV_ASSERT(subject->get() == value2);
            }
            DJV_ASSERT(0 == subject->getObserversCount());
        }
        
    } // namespace CoreTest
} // namespace djv

