//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        TagsTest::TagsTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::TagsTest", context)
        {}
        
        void TagsTest::run(const std::vector<std::string>& args)
        {
            {
                const Tags tags;
                DJV_ASSERT(tags.isEmpty());
                DJV_ASSERT(0 == tags.getCount());
                DJV_ASSERT(tags.getTags().empty());
                DJV_ASSERT(!tags.hasTag("tag"));
                DJV_ASSERT(tags.getTag("tag").empty());
            }
            
            {
                Tags tags;
                const std::map<std::string, std::string> map =
                {
                    { "a", "1" },
                    { "b", "2" },
                    { "c", "3" }
                };
                tags.setTags(map);
                DJV_ASSERT(map == tags.getTags());
                tags.setTag("d", "4");
                DJV_ASSERT(tags.hasTag("d"));
                DJV_ASSERT("4" == tags.getTag("d"));
            }
            
            {
                Tags tags;
                const std::map<std::string, std::string> map =
                {
                    { "a", "1" },
                    { "b", "2" },
                    { "c", "3" }
                };
                tags.setTags(map);
                Tags tags2;
                tags2.setTags(map);
                DJV_ASSERT(tags == tags2);
            }
        }

    } // namespace AVTest
} // namespace djv

