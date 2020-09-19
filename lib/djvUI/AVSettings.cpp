// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/AVSettings.h>

#include <djvAV/AVSystem.h>
#include <djvAV/SpeedFunc.h>
#include <djvAV/TimeFunc.h>
#include <djvAV/IOSystem.h>

#include <djvSystem/Context.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

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
            struct AV::Private
            {
                std::shared_ptr<djv::AV::IO::IOSystem> ioSystem;
                std::shared_ptr<djv::AV::AVSystem> avSystem;
            };

            void AV::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::AV", context);
                DJV_PRIVATE_PTR();
                p.ioSystem = context->getSystemT<djv::AV::IO::IOSystem>();
                p.avSystem = context->getSystemT<djv::AV::AVSystem>();
                _load();
            }

            AV::AV() :
                _p(new Private)
            {}

            AV::~AV()
            {}

            std::shared_ptr<AV> AV::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<AV>(new AV);
                out->_init(context);
                return out;
            }

            void AV::load(const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.IsObject())
                {
                    djv::AV::Time::Units timeUnits = djv::AV::Time::Units::First;
                    djv::AV::FPS defaultSpeed = djv::AV::getDefaultSpeed();
                    read("TimeUnits", value, timeUnits);
                    read("DefaultSpeed", value, defaultSpeed);

                    for (const auto& i : p.ioSystem->getPluginNames())
                    {
                        const auto j = value.FindMember(i.c_str());
                        if (j != value.MemberEnd())
                        {
                            p.ioSystem->setOptions(i, j->value);
                        }
                    }
                    p.avSystem->setTimeUnits(timeUnits);
                    p.avSystem->setDefaultSpeed(defaultSpeed);
                }
            }

            rapidjson::Value AV::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                for (const auto& i : p.ioSystem->getPluginNames())
                {
                    out.AddMember(rapidjson::Value(i.c_str(), allocator), p.ioSystem->getOptions(i, allocator), allocator);
                }
                write("TimeUnits", p.avSystem->observeTimeUnits()->get(), out, allocator);
                write("DefaultSpeed", p.avSystem->observeDefaultSpeed()->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

