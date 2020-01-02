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

#include <djvTestLib/TickTest.h>

#include <djvCore/Context.h>

#include <thread>

namespace djv
{
    namespace Test
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t frameRate = 60;
        
        } // namespace
        
        ITickTest::ITickTest(const std::string & name, const std::shared_ptr<Core::Context>& context) :
            ITest(name, context)
        {}
        
        ITickTest::~ITickTest()
        {}

        void ITickTest::_tickFor(const Core::Time::Unit& value)
        {
            if (auto context = getContext().lock())
            {
                auto time = std::chrono::steady_clock::now();
                auto timeout = time + value;
                Core::Time::Unit delta;
                while (time < timeout)
                {
                    context->tick(time, delta);

                    auto t = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<std::chrono::milliseconds>(t - time);
                    const float sleep = 1 / static_cast<float>(frameRate) - delta.count();
                    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleep * 1000)));
                    
                    t = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<std::chrono::milliseconds>(t - time);
                    time = t;
                }
            }
        }
                
    } // namespace Test
} // namespace djv

