// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/MapObserverTest.h>

#include <djvCore/MapObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        MapObserverTest::MapObserverTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::MapObserverTest", context)
        {}
        
        void MapObserverTest::run()
        {
            std::map<int, std::string> value;
            auto subject = MapSubject<int, std::string>::create(value);
            {
                std::map<int, std::string> value2;
                auto observer = MapObserver<int, std::string>::create(
                    subject,
                    [&value2](const std::map<int, std::string>& value)
                    {
                        value2 = value;
                    });
                DJV_ASSERT(1 == subject->getObserversCount());
                
                DJV_ASSERT(!subject->setIfChanged(value));
                value[1] = "one";
                subject->setAlways(value);
                DJV_ASSERT(subject->get() == value2);
                value[2] = "two";
                DJV_ASSERT(subject->setIfChanged(value));
                DJV_ASSERT(!subject->setIfChanged(value));
                DJV_ASSERT(subject->get() == value2);
                subject->setItem(2, "Two");
                DJV_ASSERT(subject->get() == value2);
                subject->setItemOnlyIfChanged(2, "too");
                subject->setItemOnlyIfChanged(2, "too");
                DJV_ASSERT(subject->get() == value2);
                
                DJV_ASSERT(2 == subject->getSize());
                DJV_ASSERT(subject->hasKey(1));
                DJV_ASSERT("one" == subject->getItem(1));

                DJV_ASSERT(!subject->isEmpty());
                subject->clear();
                DJV_ASSERT(subject->get() == value2);
                DJV_ASSERT(subject->isEmpty());
            }
            DJV_ASSERT(0 == subject->getObserversCount());
        }
        
    } // namespace CoreTest
} // namespace djv

