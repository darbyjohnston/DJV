// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvRender2DTest/Render2DSystemTest.h>

#include <djvRender2D/DataFunc.h>
#include <djvRender2D/EnumFunc.h>
#include <djvRender2D/Render2DSystem.h>

#include <djvSystem/Context.h>

#include <djvCore/ValueObserver.h>

using namespace djv::Core;
using namespace djv::Render2D;

namespace djv
{
    namespace Render2DTest
    {
        Render2DSystemTest::Render2DSystemTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::Render2DTest::Render2DSystemTest", tempPath, context)
        {}
        
        void Render2DSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<Render2DSystem>();
                
                auto alphaBlendObserver = ValueObserver<AlphaBlend>::create(
                    system->observeAlphaBlend(),
                    [this](AlphaBlend value)
                    {
                        std::stringstream ss;
                        ss << "Alpha blend: " << value;
                        _print(ss.str());
                    });
                auto imageFilterOptionsObserver = ValueObserver<ImageFilterOptions>::create(
                    system->observeImageFilterOptions(),
                    [this](const ImageFilterOptions& value)
                    {
                        std::stringstream ss;
                        ss << "Image filter: " << value.min << " " << value.mag;
                        _print(ss.str());
                    });
                auto textLCDRenderingObserver = ValueObserver<bool>::create(
                    system->observeTextLCDRendering(),
                    [this](bool value)
                    {
                        std::stringstream ss;
                        ss << "Text LCD rendering: " << value;
                        _print(ss.str());
                    });

                system->setAlphaBlend(AlphaBlend::Premultiplied);
                system->setImageFilterOptions(ImageFilterOptions(
                    ImageFilter::Nearest,
                    ImageFilter::Nearest));
                system->setTextLCDRendering(false);
            }
        }

    } // namespace Render2DTest
} // namespace djv

