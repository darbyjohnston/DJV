//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvCoreTest/AnimationTest.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>

namespace djv
{
    using namespace Core;

    namespace CoreTest
    {
        AnimationTest::AnimationTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::AnimationTest", context)
        {}
        
        void AnimationTest::run(const std::vector<std::string>& args)
        {
            for (auto i : Animation::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print(ss.str());
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
                        std::chrono::milliseconds(1000),
                        [this](float v)
                        {
                            std::stringstream ss;
                            ss << v;
                            _print(ss.str());
                        });
                    DJV_ASSERT(animation->isActive());
                    context->tick(0.F);
                    animation->stop();
                    DJV_ASSERT(!animation->isActive());
                }
            }
        }
        
    } // namespace CoreTest
} // namespace djv

