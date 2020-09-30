// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/ListObserverTest.h>

#include <djvCore/ListObserver.h>

using namespace djv::Core;
using namespace djv::Core::Observer;

namespace djv
{
    namespace CoreTest
    {
        ListObserverTest::ListObserverTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::CoreTest::ListObserverTest", tempPath, context)
        {}
        
        void ListObserverTest::run()
        {
            std::vector<int> value;
            auto subject = Observer::ListSubject<int>::create(value);
            DJV_ASSERT(0 == subject->getObserversCount());
            DJV_ASSERT(subject->isEmpty());
            DJV_ASSERT(!subject->setIfChanged(value));
            
            {
                std::vector<int> valueA;
                auto observerA = Observer::ListObserver<int>::create(
                    subject,
                    [&valueA](const std::vector<int> value)
                    {
                        valueA = value;
                    });
                DJV_ASSERT(1 == subject->getObserversCount());
                
                {
                    std::vector<int> valueB;
                    auto observerB = Observer::ListObserver<int>::create(
                        subject,
                        [&valueB](const std::vector<int> value)
                        {
                            valueB = value;
                        });
                    DJV_ASSERT(2 == subject->getObserversCount());
                    
                    value.push_back(1);
                    subject->setAlways(value);
                    DJV_ASSERT(subject->get() == valueA);
                    DJV_ASSERT(subject->get() == valueB);
                    DJV_ASSERT(!subject->isEmpty());
                    DJV_ASSERT(1 == subject->getItem(0));
                    
                    value.push_back(2);
                    DJV_ASSERT(subject->setIfChanged(value));
                    DJV_ASSERT(!subject->setIfChanged(value));
                    DJV_ASSERT(subject->get() == valueA);
                    DJV_ASSERT(subject->get() == valueB);
                    
                    DJV_ASSERT(subject->contains(2));
                    DJV_ASSERT(1 == subject->indexOf(2));
                    
                    subject->setItem(1, 3);
                    DJV_ASSERT(3 == subject->getItem(1));
                    
                    subject->setItemOnlyIfChanged(1, 4);
                    subject->setItemOnlyIfChanged(1, 4);
                    DJV_ASSERT(4 == subject->getItem(1));
                    
                    subject->pushBack(5);
                    DJV_ASSERT(5 == subject->getItem(2));
                    
                    subject->removeItem(2);
                    DJV_ASSERT(2 == subject->getSize());
                    subject->clear();
                    DJV_ASSERT(subject->isEmpty());
                }
                DJV_ASSERT(1 == subject->getObserversCount());
            }
            DJV_ASSERT(0 == subject->getObserversCount());
        }
        
    } // namespace CoreTest
} // namespace djv

