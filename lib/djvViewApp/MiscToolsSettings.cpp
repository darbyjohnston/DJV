// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MiscToolsSettings.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MiscToolsSettings::Private
        {
            std::shared_ptr<Observer::ValueSubject<bool> > messagesPopup;
            std::map<std::string, bool> debugBellowsState;
        };

        void MiscToolsSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::MiscToolsSettings", context);
            DJV_PRIVATE_PTR();
            p.messagesPopup = Observer::ValueSubject<bool>::create(true);
            _load();
        }

        MiscToolsSettings::MiscToolsSettings() :
            _p(new Private)
        {}

        MiscToolsSettings::~MiscToolsSettings()
        {}

        std::shared_ptr<MiscToolsSettings> MiscToolsSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MiscToolsSettings>(new MiscToolsSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<bool> > MiscToolsSettings::observeMessagesPopup() const
        {
            return _p->messagesPopup;
        }

        void MiscToolsSettings::setMessagesPopup(bool value)
        {
            _p->messagesPopup->setIfChanged(value);
        }

        std::map<std::string, bool> MiscToolsSettings::getDebugBellowsState() const
        {
            return _p->debugBellowsState;
        }

        void MiscToolsSettings::setDebugBellowsState(const std::map<std::string, bool>& value)
        {
            _p->debugBellowsState = value;
        }

        void MiscToolsSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("MessagesPopup", value, p.messagesPopup);
                UI::Settings::read("DebugBellowsState", value, p.debugBellowsState);
            }
        }

        rapidjson::Value MiscToolsSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("MessagesPopup", p.messagesPopup->get(), out, allocator);
            UI::Settings::write("DebugBellowsState", p.debugBellowsState, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

