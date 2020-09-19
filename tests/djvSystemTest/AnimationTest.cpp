// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystemTest/AnimationTest.h>

#include <djvSystem/Animation.h>
#include <djvSystem/AnimationFunc.h>
#include <djvSystem/Context.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::System;

namespace djv
{
    namespace SystemTest
    {
        AnimationTest::AnimationTest(
            const File::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest("djv::SystemTest::AnimationTest", tempPath, context)
        {}
        
        void AnimationTest::run()
        {
            if (auto context = getContext().lock())
            {
                for (auto i : Animation::getTypeEnums())
                {
                    auto animation = Animation::Animation::create(context);
                    DJV_ASSERT(!animation->isActive());
                    DJV_ASSERT(!animation->isRepeating());
                    animation->setType(i);
                    animation->setRepeating(true);
                    DJV_ASSERT(animation->getType() == i);
                    DJV_ASSERT(animation->isRepeating());
                    
                    animation->start(
                        0.F,
                        1.F,
                        std::chrono::milliseconds(250),
                        [this](float v)
                        {
                            std::stringstream ss;
                            ss << v;
                            _print(ss.str());
                        },
                        [this](float v)
                        {
                            std::stringstream ss;
                            ss << v;
                            _print(ss.str());
                        });
                    DJV_ASSERT(animation->isActive());
                    
                    _tickFor(std::chrono::milliseconds(300));
                    
                    animation->stop();
                    DJV_ASSERT(!animation->isActive());
                }
            }

            if (auto context = getContext().lock())
            {
                auto animation = Animation::Animation::create(context);
                animation->start(
                    1.F,
                    0.F,
                    std::chrono::milliseconds(250),
                    [this](float v)
                    {
                        std::stringstream ss;
                        ss << v;
                        _print(ss.str());
                    },
                    [this](float v)
                    {
                        std::stringstream ss;
                        ss << v;
                        _print(ss.str());
                    });
                
                _tickFor(std::chrono::milliseconds(300));
            }
        }
        
    } // namespace SystemTest
} // namespace djv

