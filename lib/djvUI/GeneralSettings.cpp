// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/GeneralSettings.h>

#include <djvSystem/Context.h>
#include <djvSystem/TextSystem.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
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
                std::shared_ptr<System::TextSystem> textSystem;
                std::shared_ptr<ValueSubject<float> > doubleClickTime;
            };

            void General::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::General", context);
                DJV_PRIVATE_PTR();

                p.textSystem = context->getSystemT<System::TextSystem>();
                p.textSystem->setCurrentLocale(p.textSystem->getSystemLocale());

                p.doubleClickTime = ValueSubject<float>::create(.5F);

                _load();
            }

            General::General() :
                _p(new Private)
            {}

            General::~General()
            {}

            std::shared_ptr<General> General::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<General>(new General);
                out->_init(context);
                return out;
            }

            const std::shared_ptr<Core::ValueSubject<float> >& General::observeDoubleClickTime() const
            {
                return _p->doubleClickTime;
            }

            void General::setDoubleClickTime(float value)
            {
                _p->doubleClickTime->setIfChanged(value);
            }

            void General::load(const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.IsObject())
                {
                    std::string currentLocale;
                    read("CurrentLocale", value, currentLocale);
                    read("DoubleClickTime", value, p.doubleClickTime);
                    p.textSystem->setCurrentLocale(currentLocale);
                }
            }

            rapidjson::Value General::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                write("CurrentLocale", p.textSystem->observeCurrentLocale()->get(), out, allocator);
                write("DoubleClickTime", p.doubleClickTime->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

