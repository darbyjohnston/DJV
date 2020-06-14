// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/AVSettings.h>

#include <djvAV/AVSystem.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

// These need to be included last on OSX.
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

            void AV::load(const rapidjson::Value & value)
            {
                DJV_PRIVATE_PTR();
                if (value.IsObject())
                {
                    djv::AV::SwapInterval swapInterval = djv::AV::SwapInterval::Default;
                    djv::Core::Time::Units timeUnits = djv::Core::Time::Units::First;
                    djv::AV::AlphaBlend alphaBlend = djv::AV::AlphaBlend::Straight;
                    Time::FPS defaultSpeed = Time::getDefaultSpeed();
                    djv::AV::Render2D::ImageFilterOptions imageFilterOptions;
                    bool textLCDRendering = true;
                    read("SwapInterval", value, swapInterval);
                    read("TimeUnits", value, timeUnits);
                    read("AlphaBlend", value, alphaBlend);
                    read("DefaultSpeed", value, defaultSpeed);
                    read("ImageFilterOptions", value, imageFilterOptions);
                    read("TextLCDRendering", value, textLCDRendering);

                    p.glfwSystem->setSwapInterval(swapInterval);
                    for (const auto & i : p.ioSystem->getPluginNames())
                    {
                        const auto j = value.FindMember(i.c_str());
                        if (j != value.MemberEnd())
                        {
                            p.ioSystem->setOptions(i, j->value);
                        }
                    }
                    p.avSystem->setTimeUnits(timeUnits);
                    p.avSystem->setAlphaBlend(alphaBlend);
                    p.avSystem->setDefaultSpeed(defaultSpeed);
                    p.avSystem->setImageFilterOptions(imageFilterOptions);
                    p.avSystem->setTextLCDRendering(textLCDRendering);
                }
            }

            rapidjson::Value AV::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                write("SwapInterval", p.glfwSystem->observeSwapInterval()->get(), out, allocator);
                for (const auto & i : p.ioSystem->getPluginNames())
                {
                    out.AddMember(rapidjson::Value(i.c_str(), allocator), p.ioSystem->getOptions(i, allocator), allocator);
                }
                write("TimeUnits", p.avSystem->observeTimeUnits()->get(), out, allocator);
                write("AlphaBlend", p.avSystem->observeAlphaBlend()->get(), out, allocator);
                write("DefaultSpeed", p.avSystem->observeDefaultSpeed()->get(), out, allocator);
                write("ImageFilterOptions", p.avSystem->observeImageFilterOptions()->get(), out, allocator);
                write("TextLCDRendering", p.avSystem->observeTextLCDRendering()->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

