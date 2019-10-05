//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvCoreTest/MapObserverTest.h>

#include <djvCore/MapObserver.h>

namespace djv
{
    using namespace Core;

    namespace CoreTest
    {
        MapObserverTest::MapObserverTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::MapObserverTest", context)
        {}
        
        void MapObserverTest::run(const std::vector<std::string>& args)
        {
            std::map<int, std::string> value;
            auto subject = MapSubject<int, std::string>::create(value);
            std::map<int, std::string> value2;
            auto observer = MapObserver<int, std::string>::create(
                subject,
                [&value2](const std::map<int, std::string>& value)
                {
                    value2 = value;
                });

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
        
    } // namespace CoreTest
} // namespace djv

