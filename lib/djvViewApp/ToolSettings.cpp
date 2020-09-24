// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolSettings.h>

#include <djvMath/BBoxFunc.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolSettings::Private
        {
            std::shared_ptr<ValueSubject<bool> > messagesPopup;
            std::map<std::string, bool> debugBellowsState;
            std::map<std::string, Math::BBox2f> widgetGeom;
        };

        void ToolSettings::_init(const std::shared_ptr<System::Context>& context)
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

        std::shared_ptr<ToolSettings> ToolSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ToolSettings>(new ToolSettings);
            out->_init(context);
            return out;
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

        const std::map<std::string, Math::BBox2f>& ToolSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ToolSettings::setWidgetGeom(const std::map<std::string, Math::BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ToolSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("MessagesPopup", value, p.messagesPopup);
                UI::Settings::read("DebugBellowsState", value, p.debugBellowsState);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value ToolSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("MessagesPopup", p.messagesPopup->get(), out, allocator);
            UI::Settings::write("DebugBellowsState", p.debugBellowsState, out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

