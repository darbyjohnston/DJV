// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/DrivesModelTest.h>

#include <djvSystem/Context.h>
#include <djvSystem/DrivesModel.h>
#include <djvSystem/Path.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        DrivesModelTest::DrivesModelTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest("djv::SystemTest::DrivesModelTest", tempPath, context)
        {}
        
        void DrivesModelTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto model = File::DrivesModel::create(context);
                
                _tickFor(std::chrono::milliseconds(1000));
                
                const auto drives = model->observeDrives()->get();
                for (const auto& i : model->observeDrives()->get())
                {
                    std::stringstream ss;
                    ss << "drive: " << i;
                    _print(ss.str());
                }
            }
        }
        
    } // namespace SystemTest
} // namespace djv

