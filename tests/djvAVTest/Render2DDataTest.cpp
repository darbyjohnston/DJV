// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/Render2DDataTest.h>

#include <djvAV/Render2DData.h>

using namespace djv::Core;
using namespace djv::AV;
using namespace djv::AV::Render2D;

namespace djv
{
    namespace AVTest
    {
        Render2DDataTest::Render2DDataTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::Render2DDataTest", tempPath, context)
        {}
        
        void Render2DDataTest::run()
        {
            {
                ImageOptions options;
                options.channelDisplay = ImageChannelDisplay::Red;
                DJV_ASSERT(options == options);
                DJV_ASSERT(options != ImageOptions());
            }
        }

    } // namespace AVTest
} // namespace djv

