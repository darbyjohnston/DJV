// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorPickerSettings.h>

#include <djvViewApp/ColorPickerData.h>
#include <djvViewApp/ColorPickerDataFunc.h>

#include <djvMath/BBoxFunc.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerSettings::Private
        {
            std::shared_ptr<Observer::ValueSubject<ColorPickerData> > data;
            std::map<std::string, Math::BBox2f> widgetGeom;
        };

        void ColorPickerSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ColorPickerSettings", context);
            DJV_PRIVATE_PTR();
            p.data = Observer::ValueSubject<ColorPickerData>::create();
            _load();
        }

        ColorPickerSettings::ColorPickerSettings() :
            _p(new Private)
        {}

        ColorPickerSettings::~ColorPickerSettings()
        {}

        std::shared_ptr<ColorPickerSettings> ColorPickerSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ColorPickerSettings>(new ColorPickerSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<ColorPickerData> > ColorPickerSettings::observeData() const
        {
            return _p->data;
        }

        void ColorPickerSettings::setData(const ColorPickerData& value)
        {
            _p->data->setIfChanged(value);
        }

        const std::map<std::string, Math::BBox2f>& ColorPickerSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ColorPickerSettings::setWidgetGeom(const std::map<std::string, Math::BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ColorPickerSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("Data", value, p.data);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value ColorPickerSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("Data", p.data->get(), out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

