// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageSettings.h>

#include <djvViewApp/ImageData.h>
#include <djvViewApp/ImageDataFunc.h>

#include <djvMath/BBoxFunc.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageSettings::Private
        {
            std::map<std::string, bool> controlsBellowsState;
            std::map<std::string, bool> colorSpaceBellowsState;
            std::shared_ptr<ValueSubject<ImageData> > data;
            std::map<std::string, Math::BBox2f> widgetGeom;
        };

        void ImageSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ImageSettings", context);

            DJV_PRIVATE_PTR();
            p.data = ValueSubject<ImageData>::create();
            _load();
        }

        ImageSettings::ImageSettings() :
            _p(new Private)
        {}

        std::shared_ptr<ImageSettings> ImageSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ImageSettings>(new ImageSettings);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> ImageSettings::getControlsBellowsState() const
        {
            return _p->controlsBellowsState;
        }

        std::map<std::string, bool> ImageSettings::getColorSpaceBellowsState() const
        {
            return _p->colorSpaceBellowsState;
        }

        void ImageSettings::setControlsBellowsState(const std::map<std::string, bool>& value)
        {
            _p->controlsBellowsState = value;
        }

        void ImageSettings::setColorSpaceBellowsState(const std::map<std::string, bool>& value)
        {
            _p->colorSpaceBellowsState = value;
        }

        std::shared_ptr<IValueSubject<ImageData> > ImageSettings::observeData() const
        {
            return _p->data;
        }

        void ImageSettings::setData(const ImageData& value)
        {
            _p->data->setIfChanged(value);
        }

        const std::map<std::string, Math::BBox2f>& ImageSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ImageSettings::setWidgetGeom(const std::map<std::string, Math::BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ImageSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("Data", value, p.data);
                UI::Settings::read("ControlsBellows", value, p.controlsBellowsState);
                UI::Settings::read("ColorSpaceBellows", value, p.colorSpaceBellowsState);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value ImageSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("Data", p.data->get(), out, allocator);
            UI::Settings::write("ControlsBellows", p.controlsBellowsState, out, allocator);
            UI::Settings::write("ColorSpaceBellows", p.colorSpaceBellowsState, out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

