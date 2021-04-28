// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/GLFWSettings.h>

#include <djvGL/GLFWSystem.h>

#include <djvSystem/Context.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct GLFW::Private
            {
                std::shared_ptr<djv::GL::GLFW::GLFWSystem> glfwSystem;
            };

            void GLFW::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::GLFW", context);
                DJV_PRIVATE_PTR();
                p.glfwSystem = context->getSystemT<djv::GL::GLFW::GLFWSystem>();
                _load();
            }

            GLFW::GLFW() :
                _p(new Private)
            {}

            GLFW::~GLFW()
            {}

            std::shared_ptr<GLFW> GLFW::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<GLFW>(new GLFW);
                out->_init(context);
                return out;
            }

            void GLFW::load(const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.IsObject())
                {
                    djv::GL::SwapInterval swapInterval = djv::GL::SwapInterval::Default;
                    read("SwapInterval", value, swapInterval);

                    p.glfwSystem->setSwapInterval(swapInterval);
                }
            }

            rapidjson::Value GLFW::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                write("SwapInterval", p.glfwSystem->observeSwapInterval()->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

