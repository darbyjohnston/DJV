// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ImageSettings.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

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
            std::shared_ptr<ValueSubject<UI::ImageRotate> > rotate;
            std::shared_ptr<ValueSubject<UI::ImageAspectRatio> > aspectRatio;
            std::map<std::string, BBox2f> widgetGeom;
        };

        void ImageSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ImageSettings", context);

            DJV_PRIVATE_PTR();
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

        std::map<std::string, bool> ImageSettings::getControlsBellowsState() const
        {
            return _p->controlsBellowsState;
        }

        void ImageSettings::setControlsBellowsState(const std::map<std::string, bool>& value)
        {
            _p->controlsBellowsState = value;
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

        void ImageSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("ControlsBellows", value, p.controlsBellowsState);
                UI::Settings::read("Rotate", value, p.rotate);
                UI::Settings::read("AspectRatio", value, p.aspectRatio);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value ImageSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("ControlsBellows", p.controlsBellowsState, out, allocator);
            UI::Settings::write("Rotate", p.rotate->get(), out, allocator);
            UI::Settings::write("AspectRatio", p.aspectRatio->get(), out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv

