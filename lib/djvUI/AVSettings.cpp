// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/AVSettings.h>

#include <djvAV/AVSystem.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct AV::Private
            {
                std::shared_ptr<djv::AV::GLFW::System> glfwSystem;
                std::shared_ptr<djv::AV::IO::System> ioSystem;
                std::shared_ptr<djv::AV::AVSystem> avSystem;
            };

            void AV::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::AV", context);
                DJV_PRIVATE_PTR();
                p.glfwSystem = context->getSystemT<djv::AV::GLFW::System>();
                p.ioSystem = context->getSystemT<djv::AV::IO::System>();
                p.avSystem = context->getSystemT<djv::AV::AVSystem>();
                _load();
            }

            AV::AV() :
                _p(new Private)
            {}

            AV::~AV()
            {}

            std::shared_ptr<AV> AV::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<AV>(new AV);
                out->_init(context);
                return out;
            }

            void AV::load(const picojson::value & value)
            {
                DJV_PRIVATE_PTR();
                if (value.is<picojson::object>())
                {
                    const auto & object = value.get<picojson::object>();
                    djv::AV::SwapInterval swapInterval = djv::AV::SwapInterval::Default;
                    djv::Core::Time::Units timeUnits = djv::Core::Time::Units::First;
                    djv::AV::AlphaBlend alphaBlend = djv::AV::AlphaBlend::Straight;
                    Time::FPS defaultSpeed = Time::getDefaultSpeed();
                    djv::AV::Render2D::ImageFilterOptions imageFilterOptions;
                    bool lcdText = false;
                    read("SwapInterval", object, swapInterval);
                    read("TimeUnits", object, timeUnits);
                    read("AlphaBlend", object, alphaBlend);
                    read("DefaultSpeed", object, defaultSpeed);
                    read("ImageFilterOptions", object, imageFilterOptions);
                    read("LCDText", object, lcdText);

                    p.glfwSystem->setSwapInterval(swapInterval);
                    for (const auto & i : p.ioSystem->getPluginNames())
                    {
                        const auto j = object.find(i);
                        if (j != object.end())
                        {
                            p.ioSystem->setOptions(i, j->second);
                        }
                    }
                    p.avSystem->setTimeUnits(timeUnits);
                    p.avSystem->setAlphaBlend(alphaBlend);
                    p.avSystem->setDefaultSpeed(defaultSpeed);
                    p.avSystem->setImageFilterOptions(imageFilterOptions);
                    p.avSystem->setLCDText(lcdText);
                }
            }

            picojson::value AV::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto & object = out.get<picojson::object>();
                write("SwapInterval", p.glfwSystem->observeSwapInterval()->get(), object);
                for (const auto & i : p.ioSystem->getPluginNames())
                {
                    object[i] = p.ioSystem->getOptions(i);
                }
                write("TimeUnits", p.avSystem->observeTimeUnits()->get(), object);
                write("AlphaBlend", p.avSystem->observeAlphaBlend()->get(), object);
                write("DefaultSpeed", p.avSystem->observeDefaultSpeed()->get(), object);
                write("ImageFilterOptions", p.avSystem->observeImageFilterOptions()->get(), object);
                write("LCDText", p.avSystem->observeLCDText()->get(), object);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

