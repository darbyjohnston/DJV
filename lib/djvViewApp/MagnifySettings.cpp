// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MagnifySettings.h>

#include <djvMath/BBox.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MagnifySettings::Private
        {
            std::shared_ptr<Observer::ValueSubject<size_t> > magnify;
            std::map<std::string, Math::BBox2f> widgetGeom;
        };

        void MagnifySettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::MagnifySettings", context);
            DJV_PRIVATE_PTR();
            p.magnify = Observer::ValueSubject<size_t>::create(2);
            _load();
        }

        MagnifySettings::MagnifySettings() :
            _p(new Private)
        {}

        MagnifySettings::~MagnifySettings()
        {}

        std::shared_ptr<MagnifySettings> MagnifySettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MagnifySettings>(new MagnifySettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::Observer::IValueSubject<size_t> > MagnifySettings::observeMagnify() const
        {
            return _p->magnify;
        }

        void MagnifySettings::setMagnify(size_t value)
        {
            _p->magnify->setIfChanged(value);
        }

        const std::map<std::string, Math::BBox2f>& MagnifySettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void MagnifySettings::setWidgetGeom(const std::map<std::string, Math::BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void MagnifySettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("Magnify", value, p.magnify);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value MagnifySettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("Magnify", p.magnify->get(), out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

