// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/RandomTest.h>

#include <djvCore/Random.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        RandomTest::RandomTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::RandomTest::RandomTest", tempPath, context)
        {}
        
        void RandomTest::run()
        {
            for (size_t i = 0; i < 10; ++i)
            {
                std::stringstream ss;
                ss << "Random 0.0-1.0: " << Random::getRandom(1.f);
                _print(ss.str());
            }

            Random::setRandomSeed(1);
            for (size_t i = 0; i < 10; ++i)
            {
                std::stringstream ss;
                ss << "Random 1.0-2.0: " << Random::getRandom(1.f, 2.f);
                _print(ss.str());
            }

            Random::setRandomSeed();
            for (size_t i = 0; i < 10; ++i)
            {
                std::stringstream ss;
                ss << "Random -1.0-1.0: " << Random::getRandom(-1.f, 1.f);
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv

