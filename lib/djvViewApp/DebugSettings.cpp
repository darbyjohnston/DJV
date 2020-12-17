// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/DebugSettings.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct DebugSettings::Private
        {
            std::map<std::string, bool> bellowsState;
        };

        void DebugSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::DebugSettings", context);
            DJV_PRIVATE_PTR();
            _load();
        }

        DebugSettings::DebugSettings() :
            _p(new Private)
        {}

        DebugSettings::~DebugSettings()
        {}

        std::shared_ptr<DebugSettings> DebugSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<DebugSettings>(new DebugSettings);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> DebugSettings::getBellowsState() const
        {
            return _p->bellowsState;
        }

        void DebugSettings::setBellowsState(const std::map<std::string, bool>& value)
        {
            _p->bellowsState = value;
        }

        void DebugSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("BellowsState", value, p.bellowsState);
            }
        }

        rapidjson::Value DebugSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("BellowsState", p.bellowsState, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

