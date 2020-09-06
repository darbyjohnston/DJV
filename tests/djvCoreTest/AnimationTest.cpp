// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/AnimationTest.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        AnimationTest::AnimationTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::CoreTest::AnimationTest", tempPath, context)
        {}
        
        void AnimationTest::run()
        {
            for (auto i : Animation::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print(_getText(ss.str()));
                for (size_t j = 0; j <= 10; ++j)
                {
                    const float v = j / static_cast<float>(10);
                    std::stringstream ss;
                    ss << "    " << v << " = " << Animation::getFunction(i)(v);
                    _print(ss.str());
                }
            }

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
        
    } // namespace CoreTest
} // namespace djv

