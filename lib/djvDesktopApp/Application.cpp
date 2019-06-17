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

#include <djvDesktopApp/Application.h>

#include <djvDesktopApp/EventSystem.h>
#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/TextSystem.h>

#include <GLFW/glfw3.h>

#include <chrono>
#include <thread>

using namespace djv::Core;

namespace djv
{
    namespace Desktop
    {
        namespace
        {
            //! \todo [1.0 S] Should this be configurable?
            const size_t frameRate = 60;
        
        } // namespace

        struct Application::Private
        {
            bool running = false;
        };

        void Application::_init(int argc, char * argv[])
        {
            Context::_init(argc, argv);

            auto glfwSystem = GLFWSystem::create(this);
            auto avSystem = AV::AVSystem::create(this);
            getSystemT<AV::IO::System>()->addDependency(glfwSystem);
            getSystemT<AV::Render::Render2D>()->addDependency(glfwSystem);

            auto uiSystem = UI::UISystem::create(glfwSystem->getDPI(), this);
            auto eventSystem = EventSystem::create(glfwSystem->getGLFWWindow(), this);
        }
        
        Application::Application() :
            _p(new Private)
        {}
        
        Application::~Application()
        {}
        
        std::unique_ptr<Application> Application::create(int argc, char * argv[])
        {
            auto out = std::unique_ptr<Application>(new Application);
            out->_init(argc, argv);
            return out;
        }

        int Application::run()
        {
            DJV_PRIVATE_PTR();
            if (auto glfwWindow = getSystemT<GLFWSystem>()->getGLFWWindow())
            {
                p.running = true;
                auto time = std::chrono::system_clock::now();
                float dt = 0.f;
                while (p.running && glfwWindow && !glfwWindowShouldClose(glfwWindow))
                {
                    glfwPollEvents();
                    tick(dt);

                    auto now = std::chrono::system_clock::now();
                    std::chrono::duration<float> delta = now - time;
                    const float sleep = 1 / static_cast<float>(frameRate) - delta.count();
                    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleep * 1000)));

                    now = std::chrono::system_clock::now();
                    delta = now - time;
                    dt = delta.count();
                    time = now;
                }
            }
            return 0;
        }

        void Application::exit()
        {
            _p->running = false;
        }

    } // namespace Desktop
} // namespace Gp
