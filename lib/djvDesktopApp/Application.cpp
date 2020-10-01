// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvDesktopApp/Application.h>

#include <djvDesktopApp/EventSystem.h>
#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/SettingsSystem.h>
#include <djvUI/UISystem.h>

#include <djvRender2D/Render.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>

#include <djvGL/GLFWSystem.h>

#include <djvSystem/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <thread>

using namespace djv::Core;

namespace djv
{
    namespace Desktop
    {
        struct Application::Private
        {
            std::shared_ptr<EventSystem> eventSystem;
        };

        void Application::_init(std::list<std::string>& args)
        {
            CmdLine::Application::_init(args);
            DJV_PRIVATE_PTR();

            // Parse the command line.
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
            GLFWSystem::create(shared_from_this());
            UI::UISystem::create(resetSettings, shared_from_this());
            p.eventSystem = EventSystem::create(getSystemT<GL::GLFW::GLFWSystem>()->getWindow(), shared_from_this());
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
            DJV_PRIVATE_PTR();
            auto avGLFWSystem = getSystemT<GL::GLFW::GLFWSystem>();
            if (auto glfwWindow = avGLFWSystem->getWindow())
            {
                glfwShowWindow(glfwWindow);
                _setRunning(true);
                while (_isRunning() && glfwWindow && !glfwWindowShouldClose(glfwWindow))
                {
                    glfwPollEvents();
                    tick();
                    glfwSwapBuffers(glfwWindow);
                }
            }
        }

        void Application::_printUsage()
        {
            auto textSystem = getSystemT<System::TextSystem>();
            std::cout << " " << textSystem->getText(DJV_TEXT("cli_ui_options")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_init_settings")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_init_settings_description")) << std::endl;
            std::cout << std::endl;

            CmdLine::Application::_printUsage();
        }

    } // namespace Desktop
} // namespace Gp
