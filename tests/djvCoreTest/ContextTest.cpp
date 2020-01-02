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

#include <djvCoreTest/ContextTest.h>

#include <djvCore/Context.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        ContextTest::ContextTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::ContextTest", context)
        {}
        
        namespace
        {
            class System : public ISystem {};
        
        } // namespace
        
        void ContextTest::run(const std::vector<std::string>& args)
        {
            if (auto context = getContext().lock())
            {
                {
                    std::stringstream ss;
                    ss << "args: " << String::join(context->getArgs(), ", ");
                    _print(ss.str());
                }
                
                {
                    std::stringstream ss;
                    ss << "name: " << context->getName();
                    _print(ss.str());
                }
                
                for (const auto& i : context->getSystems())
                {
                    std::stringstream ss;
                    ss << "system: " << i->getSystemName();
                    _print(ss.str());
                }
                
                {
                    auto resourceSystem = context->getSystemT<ResourceSystem>();
                    DJV_ASSERT(resourceSystem);
                }
                
                {
                    DJV_ASSERT(!context->getSystemT<System>());
                }
                
                auto time = std::chrono::steady_clock::now();
                std::chrono::milliseconds delta;
                for (size_t i = 0; i < 100; ++i)
                {
                    context->tick(time, delta);
                    auto now = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - time);
                    time = now;
                }
                
                {
                    std::stringstream ss;
                    ss << "fps averge: " << context->getFPSAverage();
                    _print(ss.str());
                }
            }
        }
        
    } // namespace CoreTest
} // namespace djv

