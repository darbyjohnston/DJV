// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ApplicationSettings.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ApplicationSettings::Private
        {
            std::shared_ptr<MapSubject<std::string, bool> > settingsBellows;
        };

        void ApplicationSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ApplicationSettings", context);

            DJV_PRIVATE_PTR();
            p.settingsBellows = MapSubject<std::string, bool>::create();
            _load();
        }

        ApplicationSettings::ApplicationSettings() :
            _p(new Private)
        {}

        std::shared_ptr<ApplicationSettings> ApplicationSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ApplicationSettings>(new ApplicationSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IMapSubject<std::string, bool> > ApplicationSettings::observeSettingsBellows() const
        {
            return _p->settingsBellows;
        }

        void ApplicationSettings::setSettingsBellows(const std::map<std::string, bool>& value)
        {
            _p->settingsBellows->setIfChanged(value);
        }

        void ApplicationSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("SettingsBellows", object, p.settingsBellows);
            }
        }

        picojson::value ApplicationSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("SettingsBellows", p.settingsBellows->get(), object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

