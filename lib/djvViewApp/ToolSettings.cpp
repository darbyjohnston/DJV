// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolSettings.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolSettings::Private
        {
            bool infoBellowsState = true;
            std::shared_ptr<ValueSubject<bool> > messagesPopup;
            std::map<std::string, bool> debugBellowsState;
            std::map<std::string, BBox2f> widgetGeom;
        };

        void ToolSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ToolSettings", context);
            DJV_PRIVATE_PTR();
            p.messagesPopup = ValueSubject<bool>::create(true);
            _load();
        }

        ToolSettings::ToolSettings() :
            _p(new Private)
        {}

        ToolSettings::~ToolSettings()
        {}

        std::shared_ptr<ToolSettings> ToolSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ToolSettings>(new ToolSettings);
            out->_init(context);
            return out;
        }

        bool ToolSettings::getInfoBellowsState() const
        {
            return _p->infoBellowsState;
        }

        void ToolSettings::setInfoBellowsState(bool value)
        {
            _p->infoBellowsState = value;
        }

        std::shared_ptr<IValueSubject<bool> > ToolSettings::observeMessagesPopup() const
        {
            return _p->messagesPopup;
        }

        void ToolSettings::setMessagesPopup(bool value)
        {
            _p->messagesPopup->setIfChanged(value);
        }

        std::map<std::string, bool> ToolSettings::getDebugBellowsState() const
        {
            return _p->debugBellowsState;
        }

        void ToolSettings::setDebugBellowsState(const std::map<std::string, bool>& value)
        {
            _p->debugBellowsState = value;
        }

        const std::map<std::string, BBox2f>& ToolSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ToolSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ToolSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("InfoBellowsState", object, p.infoBellowsState);
                UI::Settings::read("MessagesPopup", object, p.messagesPopup);
                UI::Settings::read("DebugBellowsState", object, p.debugBellowsState);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value ToolSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("InfoBellowsState", p.infoBellowsState, object);
            UI::Settings::write("MessagesPopup", p.messagesPopup->get(), object);
            UI::Settings::write("DebugBellowsState", p.debugBellowsState, object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

