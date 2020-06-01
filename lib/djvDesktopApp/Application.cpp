// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvDesktopApp/EventSystem.h>
#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/SettingsSystem.h>
#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
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
            //! \todo Should this be configurable?
            const size_t frameRate = 60;
            const size_t spinLoopSleep = 100;
        
        } // namespace

        struct Application::Private
        {};

        void Application::_init(std::list<std::string>& args)
        {
            CmdLine::Application::_init(args);

            bool resetSettings = false;
            auto arg = args.begin();
            while (arg != args.end())
            {
                if ("-init_settings" == *arg)
                {
                    arg = args.erase(arg);
                    resetSettings = true;
                }
                else
                {
                    ++arg;
                }
            }

            // Create the systems.
            auto glfwSystem = GLFWSystem::create(shared_from_this());
            auto uiSystem = UI::UISystem::create(resetSettings, shared_from_this());
            auto avGLFWSystem = getSystemT<AV::GLFW::System>();
            auto glfwWindow = avGLFWSystem->getGLFWWindow();
            auto eventSystem = EventSystem::create(glfwWindow, shared_from_this());
        }
        
        Application::Application() :
            _p(new Private)
        {}
        
        Application::~Application()
        {}

        std::shared_ptr<Application> Application::create(std::list<std::string>& args)
        {
            auto out = std::shared_ptr<Application>(new Application);
            out->_init(args);
            return out;
        }

        void Application::run()
        {
            auto avGLFWSystem = getSystemT<AV::GLFW::System>();
            if (auto glfwWindow = avGLFWSystem->getGLFWWindow())
            {
                glfwShowWindow(glfwWindow);
                auto start = std::chrono::steady_clock::now();
                auto delta = Time::Duration::zero();
                auto frameTime = std::chrono::microseconds(1000000 / frameRate);
                _setRunning(true);
                while (_isRunning() && glfwWindow && !glfwWindowShouldClose(glfwWindow))
                {
                    glfwPollEvents();
                    tick();
                    //const auto systemTime = std::chrono::steady_clock::now();

                    auto end = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<Time::Duration>(end - start);
                    //Time::sleep(frameTime - delta);
                    while (delta < frameTime)
                    {
                        Time::sleep(std::chrono::microseconds(spinLoopSleep));
                        tickTimers();
                        end = std::chrono::steady_clock::now();
                        delta = std::chrono::duration_cast<Time::Duration>(end - start);
                    }
                    end = std::chrono::steady_clock::now();
                    //delta = std::chrono::duration_cast<Time::Unit>(end - start);
                    //std::cout << "frame: " <<
                    //    std::chrono::duration_cast<Time::Unit>(systemTime - start).count() << "/" <<
                    //    delta.count() << "/" <<
                    //    (1000000 / frameRate) << std::endl;
                    start = end;
                }
            }
        }

        void Application::_printUsage()
        {
            auto textSystem = getSystemT<Core::TextSystem>();
            std::cout << " " << textSystem->getText(DJV_TEXT("cli_ui_options")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_init_settings")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_init_settings_description")) << std::endl;
            std::cout << std::endl;

            CmdLine::Application::_printUsage();
        }

    } // namespace Desktop
} // namespace Gp
