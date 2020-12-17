// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MessagesSettings.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MessagesSettings::Private
        {
            std::shared_ptr<Observer::ValueSubject<bool> > popup;
        };

        void MessagesSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::MessagesSettings", context);
            DJV_PRIVATE_PTR();
            p.popup = Observer::ValueSubject<bool>::create(true);
            _load();
        }

        MessagesSettings::MessagesSettings() :
            _p(new Private)
        {}

        MessagesSettings::~MessagesSettings()
        {}

        std::shared_ptr<MessagesSettings> MessagesSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MessagesSettings>(new MessagesSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<bool> > MessagesSettings::observePopup() const
        {
            return _p->popup;
        }

        void MessagesSettings::setPopup(bool value)
        {
            _p->popup->setIfChanged(value);
        }

        void MessagesSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("Popup", value, p.popup);
            }
        }

        rapidjson::Value MessagesSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("Popup", p.popup->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

