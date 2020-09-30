// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvRender2DTest/RenderSystemTest.h>

#include <djvRender2D/DataFunc.h>
#include <djvRender2D/EnumFunc.h>
#include <djvRender2D/RenderSystem.h>

#include <djvSystem/Context.h>

#include <djvCore/ValueObserver.h>

using namespace djv::Core;
using namespace djv::Render2D;

namespace djv
{
    namespace Render2DTest
    {
        RenderSystemTest::RenderSystemTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::Render2DTest::RenderSystemTest", tempPath, context)
        {}
        
        void RenderSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<RenderSystem>();
                
                auto imageFilterOptionsObserver = Observer::ValueObserver<ImageFilterOptions>::create(
                    system->observeImageFilterOptions(),
                    [this](const ImageFilterOptions& value)
                    {
                        std::stringstream ss;
                        ss << "Image filter: " << value.min << " " << value.mag;
                        _print(ss.str());
                    });
                auto textLCDRenderingObserver = Observer::ValueObserver<bool>::create(
                    system->observeTextLCDRendering(),
                    [this](bool value)
                    {
                        std::stringstream ss;
                        ss << "Text LCD rendering: " << value;
                        _print(ss.str());
                    });

                system->setImageFilterOptions(ImageFilterOptions(
                    ImageFilter::Nearest,
                    ImageFilter::Nearest));
                system->setTextLCDRendering(false);
            }
        }

    } // namespace Render2DTest
} // namespace djv

