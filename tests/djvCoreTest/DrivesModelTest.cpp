// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/DrivesModelTest.h>

#include <djvCore/Context.h>
#include <djvCore/DrivesModel.h>
#include <djvCore/Path.h>

#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        DrivesModelTest::DrivesModelTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::CoreTest::DrivesModelTest", tempPath, context)
        {}
        
        void DrivesModelTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto model = FileSystem::DrivesModel::create(context);
                
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
        
    } // namespace CoreTest
} // namespace djv

