// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvRender2DTest/DataTest.h>

#include <djvRender2D/Data.h>

using namespace djv::Core;
using namespace djv::Render2D;

namespace djv
{
    namespace Render2DTest
    {
        DataTest::DataTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::Render2DTest::DataTest", tempPath, context)
        {}
        
        void DataTest::run()
        {
            {
                ImageOptions options;
                options.channelDisplay = ImageChannelDisplay::Red;
                DJV_ASSERT(options == options);
                DJV_ASSERT(options != ImageOptions());
            }
        }

    } // namespace Render2DTest
} // namespace djv

