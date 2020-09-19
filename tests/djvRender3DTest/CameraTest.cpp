// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvRender3DTest/CameraTest.h>

#include <djvRender3D/Camera.h>

using namespace djv::Core;
using namespace djv::Render3D;

namespace djv
{
    namespace Render3DTest
    {
        CameraTest::CameraTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::Render3DTest::CameraTest", tempPath, context)
        {}
        
        void CameraTest::run()
        {
            {
                auto camera = DefaultCamera::create();
                
                const glm::mat4x4 m(1.F);
                camera->setV(m);
                DJV_ASSERT(m == camera->getV());
                camera->setP(m);
                DJV_ASSERT(m == camera->getP());
            }
        }

    } // namespace Render3DTest
} // namespace djv

