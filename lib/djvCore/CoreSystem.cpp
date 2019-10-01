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

#include <djvCore/CoreSystem.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace Core
    {
        struct CoreSystem::Private
        {
        };

        void CoreSystem::_init(const std::string&, const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::Core::CoreSystem", context);

            auto animationSystem = Animation::System::create(context);

            addDependency(animationSystem);
        }

        CoreSystem::CoreSystem() :
            _p(new Private)
        {}

        CoreSystem::~CoreSystem()
        {}

        std::shared_ptr<CoreSystem> CoreSystem::create(const std::string & argv0, const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<CoreSystem>(new CoreSystem);
            out->_init(argv0, context);
            return out;
        }

    } // namespace Core
} // namespace djv

