// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageSettings.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageSettings::Private
        {
            std::shared_ptr<MapSubject<std::string, bool> > controlControlsBellows;
            int colorSpaceCurrentTab = 0;
            int colorCurrentTab = 0;
            std::shared_ptr<ValueSubject<UI::ImageRotate> > rotate;
            std::shared_ptr<ValueSubject<UI::ImageAspectRatio> > aspectRatio;
            std::map<std::string, BBox2f> widgetGeom;
        };

        void ImageSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ImageSettings", context);

            DJV_PRIVATE_PTR();
            p.controlControlsBellows = MapSubject<std::string, bool>::create({ { "Adjustments", true } });
            p.rotate = ValueSubject<UI::ImageRotate>::create(UI::ImageRotate::_0);
            p.aspectRatio = ValueSubject<UI::ImageAspectRatio>::create(UI::ImageAspectRatio::FromSource);
            _load();
        }

        ImageSettings::ImageSettings() :
            _p(new Private)
        {}

        std::shared_ptr<ImageSettings> ImageSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ImageSettings>(new ImageSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IMapSubject<std::string, bool> > ImageSettings::observeColorControlsBellows() const
        {
            return _p->controlControlsBellows;
        }

        void ImageSettings::setColorControlsBellows(const std::map<std::string, bool>& value)
        {
            _p->controlControlsBellows->setIfChanged(value);
        }

        int ImageSettings::getColorSpaceCurrentTab() const
        {
            return _p->colorSpaceCurrentTab;
        }

        int ImageSettings::getColorCurrentTab() const
        {
            return _p->colorCurrentTab;
        }

        void ImageSettings::setColorSpaceCurrentTab(int value)
        {
            _p->colorSpaceCurrentTab = value;
        }

        void ImageSettings::setColorCurrentTab(int value)
        {
            _p->colorCurrentTab = value;
        }

        std::shared_ptr<IValueSubject<UI::ImageRotate> > ImageSettings::observeRotate() const
        {
            return _p->rotate;
        }

        std::shared_ptr<IValueSubject<UI::ImageAspectRatio> > ImageSettings::observeAspectRatio() const
        {
            return _p->aspectRatio;
        }

        void ImageSettings::setRotate(UI::ImageRotate value)
        {
            _p->rotate->setIfChanged(value);
        }

        void ImageSettings::setAspectRatio(UI::ImageAspectRatio value)
        {
            _p->aspectRatio->setIfChanged(value);
        }

        const std::map<std::string, BBox2f>& ImageSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ImageSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ImageSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("ColorControlsBellows", object, p.controlControlsBellows);
                UI::Settings::read("ColorSpaceCurrentTab", object, p.colorSpaceCurrentTab);
                UI::Settings::read("ColorCurrentTab", object, p.colorCurrentTab);
                UI::Settings::read("Rotate", object, p.rotate);
                UI::Settings::read("AspectRatio", object, p.aspectRatio);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value ImageSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("ColorControlsBellows", p.controlControlsBellows->get(), object);
            UI::Settings::write("ColorSpaceCurrentTab", p.colorSpaceCurrentTab, object);
            UI::Settings::write("ColorCurrentTab", p.colorCurrentTab, object);
            UI::Settings::write("Rotate", p.rotate->get(), object);
            UI::Settings::write("AspectRatio", p.aspectRatio->get(), object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

