// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotateSettings.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateSettings::Private
        {
            std::map<std::string, BBox2f> widgetGeom;
        };

        void AnnotateSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::AnnotateSettings", context);
            _load();
        }

        AnnotateSettings::AnnotateSettings() :
            _p(new Private)
        {}

        AnnotateSettings::~AnnotateSettings()
        {}

        std::shared_ptr<AnnotateSettings> AnnotateSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<AnnotateSettings>(new AnnotateSettings);
            out->_init(context);
            return out;
        }

        const std::map<std::string, BBox2f>& AnnotateSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void AnnotateSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void AnnotateSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value AnnotateSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

