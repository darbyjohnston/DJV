// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/Render3DCameraTest.h>

#include <djvAV/Render3DCamera.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        Render3DCameraTest::Render3DCameraTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::Render3DCameraTest", tempPath, context)
        {}
        
        void Render3DCameraTest::run()
        {
            {
                auto camera = Render3D::DefaultCamera::create();
                
                const glm::mat4x4 m(1.F);
                camera->setV(m);
                DJV_ASSERT(m == camera->getV());
                camera->setP(m);
                DJV_ASSERT(m == camera->getP());
            }
        }

    } // namespace AVTest
} // namespace djv

