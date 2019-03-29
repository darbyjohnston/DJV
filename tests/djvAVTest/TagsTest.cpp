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

#include <djvAVTest/TagsTest.h>

#include <djvAV/Tags.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>

#include <QStringList>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        void TagsTest::run(int &, char **)
        {
            DJV_DEBUG("TagsTest::run");
            ctors();
            members();
            operators();
        }

        void TagsTest::ctors()
        {
            DJV_DEBUG("TagsTest::ctors");
            {
                const AV::Tags tags;
                DJV_ASSERT(0 == tags.count());
            }
            {
                AV::Tags tmp;
                tmp.add("key", "value");
                const AV::Tags tags(tmp);
                DJV_ASSERT(1 == tags.count());
                DJV_ASSERT("key" == tags.keys()[0]);
                DJV_ASSERT("value" == tags.values()[0]);
            }
        }

        void TagsTest::members()
        {
            DJV_DEBUG("TagsTest::members");
            {
                AV::Tags tmp;
                tmp.add("key", "value");
                AV::Tags tags;
                tags.add(tmp);
                DJV_ASSERT(1 == tags.count());
                DJV_ASSERT("key" == tags.keys()[0]);
                DJV_ASSERT("value" == tags.values()[0]);
                DJV_ASSERT("value" == tags.tag("key"));
                DJV_ASSERT(!tags.tag("none").length());
                DJV_ASSERT(tags.isValid("key"));
                DJV_ASSERT(!tags.isValid("none"));
                tags.add("key", "value 2");
                DJV_ASSERT(1 == tags.count());
                DJV_ASSERT("value 2" == tags.tag("key"));
                tags.clear();
                DJV_ASSERT(0 == tags.count());
            }
            {
                DJV_DEBUG_PRINT(AV::Tags::tagLabels());
            }
        }

        void TagsTest::operators()
        {
            DJV_DEBUG("TagsTest::operators");
            {
                AV::Tags tags;
                tags["key"] = "value";
                DJV_ASSERT("value" == tags["key"]);
                const AV::Tags & tmp = tags;
                DJV_ASSERT("value" == tmp["key"]);
                tags["key 2"];
                DJV_ASSERT(2 == tags.count());
            }
            {
                AV::Tags a, b, c;
                a["key"] = "value";
                b["key"] = "value";
                c["key"] = "value 2";
                DJV_ASSERT(a == b);
                DJV_ASSERT(a != c);
                DJV_ASSERT(a != AV::Tags());
            }
            {
                AV::Tags tags;
                tags["key"] = "value";
                tags["key 2"] = "value 2";
                DJV_DEBUG_PRINT(tags);
            }
        }

    } // namespace AVTest
} // namespace djv
