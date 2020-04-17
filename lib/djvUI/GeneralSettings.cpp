// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/GeneralSettings.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct General::Private
            {
                std::shared_ptr<TextSystem> textSystem;
            };

            void General::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::General", context);
                DJV_PRIVATE_PTR();
                p.textSystem = context->getSystemT<TextSystem>();
                p.textSystem->setCurrentLocale(p.textSystem->getSystemLocale());
                _load();
            }

            General::General() :
                _p(new Private)
            {}

            General::~General()
            {}

            std::shared_ptr<General> General::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<General>(new General);
                out->_init(context);
                return out;
            }

            void General::load(const picojson::value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.is<picojson::object>())
                {
                    const auto & object = value.get<picojson::object>();
                    std::string currentLocale;
                    read("CurrentLocale", object, currentLocale);
                    p.textSystem->setCurrentLocale(currentLocale);
                }
            }

            picojson::value General::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto & object = out.get<picojson::object>();
                write("CurrentLocale", p.textSystem->observeCurrentLocale()->get(), object);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

