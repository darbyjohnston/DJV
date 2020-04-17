// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/ListObserverTest.h>

#include <djvCore/ListObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        ListObserverTest::ListObserverTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ListObserverTest", context)
        {}
        
        void ListObserverTest::run()
        {
            std::vector<int> value;
            auto subject = ListSubject<int>::create(value);
            {
                std::vector<int> value2;
                auto observer = ListObserver<int>::create(
                    subject,
                    [&value2](const std::vector<int> value)
                    {
                        value2 = value;
                    });
                DJV_ASSERT(1 == subject->getObserversCount());
                    
                DJV_ASSERT(!subject->setIfChanged(value));
                value.push_back(1);
                subject->setAlways(value);
                DJV_ASSERT(subject->get() == value2);
                DJV_ASSERT(!subject->isEmpty());
                DJV_ASSERT(1 == subject->getItem(0));
                value.push_back(2);
                DJV_ASSERT(subject->setIfChanged(value));
                DJV_ASSERT(!subject->setIfChanged(value));
                DJV_ASSERT(subject->get() == value2);
                
                DJV_ASSERT(subject->contains(2));
                DJV_ASSERT(1 == subject->indexOf(2));
            }
            DJV_ASSERT(0 == subject->getObserversCount());
        }
        
    } // namespace CoreTest
} // namespace djv

