// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ApplicationSettings.h>

#include <djvSystem/Context.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ApplicationSettings::Private
        {
            std::shared_ptr<Observer::MapSubject<std::string, bool> > settingsBellows;
        };

        void ApplicationSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ApplicationSettings", context);

            DJV_PRIVATE_PTR();
            p.settingsBellows = Observer::MapSubject<std::string, bool>::create();
            _load();
        }

        ApplicationSettings::ApplicationSettings() :
            _p(new Private)
        {}

        std::shared_ptr<ApplicationSettings> ApplicationSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ApplicationSettings>(new ApplicationSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Observer::IMapSubject<std::string, bool> > ApplicationSettings::observeSettingsBellows() const
        {
            return _p->settingsBellows;
        }

        void ApplicationSettings::setSettingsBellows(const std::map<std::string, bool>& value)
        {
            _p->settingsBellows->setIfChanged(value);
        }

        void ApplicationSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("SettingsBellows", value, p.settingsBellows);
            }
        }

        rapidjson::Value ApplicationSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("SettingsBellows", p.settingsBellows->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

