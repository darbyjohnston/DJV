// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/ValueObserverTest.h>

#include <djvCore/ValueObserver.h>

using namespace djv::Core;
using namespace djv::Core::Observer;

namespace djv
{
    namespace CoreTest
    {
        ValueObserverTest::ValueObserverTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::CoreTest::ValueObserverTest", tempPath, context)
        {}
        
        void ValueObserverTest::run()
        {
            int value = 0;
            auto subject = Observer::ValueSubject<int>::create(value);
            DJV_ASSERT(0 == subject->getObserversCount());
            DJV_ASSERT(!subject->setIfChanged(value));
            
            {
                int valueA = 0;
                auto observer = Observer::Value<int>::create(
                    subject,
                    [&valueA](int value)
                    {
                        valueA = value;
                    });
                DJV_ASSERT(1 == subject->getObserversCount());

                {
                    int valueB = 0;
                    auto observer = Observer::Value<int>::create(
                        subject,
                        [&valueB](int value)
                        {
                            valueB = value;
                        });
                    DJV_ASSERT(2 == subject->getObserversCount());

                    value = 1;
                    subject->setAlways(value);
                    DJV_ASSERT(subject->get() == valueA);
                    DJV_ASSERT(subject->get() == valueB);
                    
                    value = 2;
                    DJV_ASSERT(subject->setIfChanged(value));
                    DJV_ASSERT(!subject->setIfChanged(value));
                    DJV_ASSERT(subject->get() == valueA);
                    DJV_ASSERT(subject->get() == valueB);
                }
                DJV_ASSERT(1 == subject->getObserversCount());
            }
            DJV_ASSERT(0 == subject->getObserversCount());
        }
        
    } // namespace CoreTest
} // namespace djv

