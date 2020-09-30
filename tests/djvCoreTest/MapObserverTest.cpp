// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/MapObserverTest.h>

#include <djvCore/MapObserver.h>

using namespace djv::Core;
using namespace djv::Core::Observer;

namespace djv
{
    namespace CoreTest
    {
        MapObserverTest::MapObserverTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::CoreTest::MapObserverTest", tempPath, context)
        {}
        
        void MapObserverTest::run()
        {
            std::map<int, std::string> value;
            auto subject = Observer::MapSubject<int, std::string>::create(value);
            DJV_ASSERT(0 == subject->getObserversCount());
            DJV_ASSERT(subject->isEmpty());
            DJV_ASSERT(!subject->setIfChanged(value));
            
            {
                std::map<int, std::string> valueA;
                auto observer = Observer::MapObserver<int, std::string>::create(
                    subject,
                    [&valueA](const std::map<int, std::string>& value)
                    {
                        valueA = value;
                    });
                DJV_ASSERT(1 == subject->getObserversCount());
                
                {
                    std::map<int, std::string> valueB;
                    auto observer = Observer::MapObserver<int, std::string>::create(
                        subject,
                        [&valueB](const std::map<int, std::string>& value)
                        {
                            valueB = value;
                        });
                    DJV_ASSERT(2 == subject->getObserversCount());

                    value[1] = "one";
                    subject->setAlways(value);
                    DJV_ASSERT(subject->get() == valueA);
                    DJV_ASSERT(subject->get() == valueB);
                    
                    value[2] = "two";
                    DJV_ASSERT(subject->setIfChanged(value));
                    DJV_ASSERT(!subject->setIfChanged(value));
                    DJV_ASSERT(subject->get() == valueA);
                    DJV_ASSERT(subject->get() == valueB);
                    
                    subject->setItem(2, "Two");
                    subject->setItemOnlyIfChanged(2, "too");
                    subject->setItemOnlyIfChanged(2, "too");
                    DJV_ASSERT(2 == subject->getSize());
                    DJV_ASSERT(subject->hasKey(1));
                    DJV_ASSERT("one" == subject->getItem(1));

                    subject->clear();
                    DJV_ASSERT(subject->isEmpty());
                }
                DJV_ASSERT(1 == subject->getObserversCount());
            }
            DJV_ASSERT(0 == subject->getObserversCount());
        }
        
    } // namespace CoreTest
} // namespace djv

