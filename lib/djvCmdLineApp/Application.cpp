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

#include <djvCmdLineApp/Application.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>

using namespace djv::Core;

namespace djv
{
    namespace CmdLine
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t frameRate = 60;

        } // namespace

        struct Application::Private
        {
            bool running = true;
            int exit = 0;
        };

        void Application::_init(const std::vector<std::string>& args)
        {
            Context::_init(args);
            auto avSystem = AV::AVSystem::create(shared_from_this());
        }

        Application::Application() :
            _p(new Private)
        {}

        Application::~Application()
        {}

        std::shared_ptr<Application> Application::create(const std::vector<std::string>& args)
        {
            auto out = std::shared_ptr<Application>(new Application);
            out->_init(args);
            return out;
        }

        int Application::run()
        {
            DJV_PRIVATE_PTR();
            auto time = std::chrono::steady_clock::now();
            Time::Unit delta = Time::Unit::zero();
            while (p.running)
            {
                tick(time, delta);

                auto end = std::chrono::steady_clock::now();
                delta = std::chrono::duration_cast<Time::Unit>(end - time);
                while (delta.count() < Time::timebase / frameRate)
                {
                    end = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<Time::Unit>(end - time);
                }
                time = end;
            }
            return _p->exit;
        }

        void Application::exit(int value)
        {
            DJV_PRIVATE_PTR();
            p.running = false;
            p.exit = value;
        }

    } // namespace CmdLine
} // namespace djv

