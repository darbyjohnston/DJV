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

#include <djvCoreTest/StringUtilTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Math.h>
#include <djvCore/StringUtil.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void StringUtilTest::run(int &, char **)
        {
            DJV_DEBUG("StringUtilTest::run");
            find();
            cString();
            convert();
            operators();
        }

        void StringUtilTest::find()
        {
            DJV_DEBUG("StringUtilTest::find");
            const QStringList list = QStringList() << "abc" << "def" << "ghi";
            int index = 0;
            DJV_ASSERT(StringUtil::find("def", list, &index));
            DJV_ASSERT(1 == index);
            DJV_ASSERT(!StringUtil::find("", list, &index));
            DJV_ASSERT(StringUtil::findNoCase("GHI", list, &index));
            DJV_ASSERT(2 == index);
            DJV_ASSERT(!StringUtil::findNoCase("", list, &index));
        }

        void StringUtilTest::cString()
        {
            DJV_DEBUG("StringUtilTest::cString");
            const struct Data
            {
                QString s;
                QString result;
                int     maxLen;
            }
            data[] =
            {
                { "",    "",    0 },
                { "abc", "abc", 0 },
                { "abc", "a",   2 }
            };
            const int dataCount = sizeof(data) / sizeof(data[0]);
            for (int i = 0; i < dataCount; ++i)
            {
                char c[StringUtil::cStringLength];
                StringUtil::cString(data[i].s, c, data[i].maxLen);
                DJV_ASSERT(data[i].result == QString(c));
            }
        }

        void StringUtilTest::convert()
        {
            DJV_DEBUG("StringUtilTest::convert");
            {
                QString v;
                QStringList tmp;
                DJV_ASSERT(!Serialize(tmp, v));
                tmp += "test";
                DJV_ASSERT(Serialize(tmp, v));
                DJV_ASSERT("test" == v);
            }
            {
                const QStringList labels = QStringList() << "true" << "false";
                QStringList tmp = labels;
                tmp += "undef";
                unsigned int i = 0;
                DJV_ASSERT(Serialize(tmp, i, labels));
                DJV_ASSERT(0 == i);
                DJV_ASSERT(Serialize(tmp, i, labels));
                DJV_ASSERT(1 == i);
                DJV_ASSERT(!Serialize(tmp, i, labels));
                tmp = Serialize<int>(0, labels);
                DJV_ASSERT(tmp.count());
                DJV_ASSERT("true" == tmp[0]);
                tmp = Serialize<int>(2, labels);
                DJV_ASSERT(!tmp.count());
            }
        }

        void StringUtilTest::operators()
        {
            DJV_DEBUG("StringUtilTest::operators");
            {
                QString v = "test";
                QStringList tmp;
                tmp << v;
                tmp >> v;
                DJV_ASSERT("test" == v);
                try
                {
                    tmp >> v;
                    DJV_ASSERT(0);
                }
                catch (...)
                {
                }
            }
            {
                QStringList v = QStringList() << "a" << "b" << "c";
                QStringList tmp;
                tmp << v;
                tmp >> v;
                DJV_ASSERT((QStringList() << "a" << "b" << "c") == v);
            }
            {
                bool v = true;
                QStringList tmp;
                tmp << v;
                tmp >> v;
                DJV_ASSERT(true == v);
                try
                {
                    tmp >> v;
                    DJV_ASSERT(0);
                }
                catch (...)
                {
                }
            }
            {
                int v = 1;
                QStringList tmp;
                tmp << v;
                tmp >> v;
                DJV_ASSERT(1 == v);
            }
            {
                unsigned int v = 1;
                QStringList tmp;
                tmp << v;
                tmp >> v;
                DJV_ASSERT(1 == v);
            }
            {
                float v = 1.f;
                QStringList tmp;
                tmp << v;
                tmp >> v;
                DJV_ASSERT(Math::fuzzyCompare(1.f, v));
            }
        }

    } // namespace CoreTest
} // namespace djv
