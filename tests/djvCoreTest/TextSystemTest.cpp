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

#include <djvCoreTest/TextSystemTest.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        TextSystemTest::TextSystemTest(const std::shared_ptr<Core::Context>& context) :
            ITickTest("djv::CoreTest::TextSystemTest", context)
        {}
                
        void TextSystemTest::run(const std::vector<std::string>&)
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<TextSystem>();
                
                for (const auto& i : system->getLocales())
                {
                    std::stringstream ss;
                    ss << "locale: " << i;
                    _print(ss.str());
                }

                auto localeObserver = ValueObserver<std::string>::create(
                    system->observeCurrentLocale(),
                    [this](const std::string& value)
                    {
                        std::stringstream ss;
                        ss << "current locale: " << value;
                        _print(ss.str());
                    });
                
                {
                    system->setCurrentLocale("zh");
                    system->setCurrentLocale("zh");
                    DJV_ASSERT("zh" == system->observeCurrentLocale()->get());
                }
                
                for (const std::string& i : { "File", "Window", "Image" })
                {
                    std::stringstream ss;
                    ss << i << ": " << system->getText(i);
                    _print(ss.str());
                }
            }
        }
                
    } // namespace CoreTest
} // namespace djv

